#include "bluetoothlistener.hpp"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

#include "devicechooser.hpp"

BluetoothListener::BluetoothListener(ScreenLocker &screenLocker, QObject *parent)
    : QObject{parent}
    , m_screenLocker(screenLocker)
{
    if (not m_localDevice.isValid()) {
        auto reply = QMessageBox::question(nullptr,
                                           tr("Bluetooth is off"),
                                           tr("For this program to work, you have to have bluetooth on\n"
                                              "Would you like to turn it on?"));
        if (reply != QMessageBox::Yes) {
            std::exit(1);
        }

        m_localDevice.powerOn();
    }

    m_lookForTrustedDeviceAgent = new QBluetoothDeviceDiscoveryAgent(this);

    m_deviceDiscoverTimer.setInterval(30'000); /* Discover devices for 30 seconds. */
    m_lookForTrustedDeviceTimer.setInterval(10'000); /* Check if trusted device is near every 10 seconds. */

    connect(&m_deviceDiscoverTimer, &QTimer::timeout, this, &BluetoothListener::discoverDevicesTimeout);
    connect(&m_lookForTrustedDeviceTimer, &QTimer::timeout, this, &BluetoothListener::checkForTrustedDevice);
    connect(&m_localDevice, &QBluetoothLocalDevice::hostModeStateChanged, this, &BluetoothListener::hostModeStateChanged);

    auto filename = QString("%1%2%3%4%5.ini")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation),
                             QDir::separator(),
                             PROJECT_NAME,
                             QDir::separator(),
                             PROJECT_NAME
                             );

    m_settings = new QSettings(filename, QSettings::IniFormat, this);
    m_settings->beginGroup("TrustedDevices");

    if (m_settings->allKeys().isEmpty()) {
        startDiscovery();
    } else {
        for (const auto deviceName : m_settings->allKeys()) {
            auto address = QBluetoothAddress(m_settings->value(deviceName).toString());
            m_trustedDevices.append(QBluetoothDeviceInfo(address, deviceName, 0));
        }

        checkForTrustedDevice(); /* Make call manually, then let timer call it automatically. */
    }
}

BluetoothListener::~BluetoothListener()
{
    m_settings->endGroup();
}

void BluetoothListener::startDiscovery()
{
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::DiscoveryMethod::ClassicMethod);
    m_deviceDiscoverTimer.start();

    QMessageBox::information(nullptr, tr("Discoverying devices"),
                             tr("Please wait up to 30 seconds, I'll show you the devices I discover."));
}

void BluetoothListener::hostModeStateChanged(QBluetoothLocalDevice::HostMode state)
{
    if (state == QBluetoothLocalDevice::HostPoweredOff) {
        auto reply = QMessageBox::question(nullptr,
                                           tr("Bluetooth is off"),
                                           tr("For this program to work, you have to have bluetooth on\n"
                                              "Would you like to turn it on?"));
        if (reply != QMessageBox::Yes) {
            QMessageBox::critical(nullptr, tr("Quitting"), tr("Nothing else to do."));
            std::exit(1);
        }

        m_localDevice.powerOn();
    }
}

void BluetoothListener::discoverDevicesTimeout()
{
    m_discoveryAgent->stop();

    QEventLoop loop;
    DeviceChooser chooser;
    connect(&chooser, &DeviceChooser::closed, &loop, &QEventLoop::quit);

    for (const auto &device : m_discoveryAgent->discoveredDevices()) {
        if (device.rssi() == 0) { /* Paired devices will be shown even when they're not near. */
            continue;
        }

        chooser.addDevice(device.name(), device.address().toString(), QString::number(device.rssi()));
    }

    chooser.show();
    loop.exec();

    auto device = chooser.selectedDevice();
    for (const auto &[name, address] : device) {
        m_settings->setValue(name, address);
    }
}

void BluetoothListener::checkForTrustedDeviceScanCompleted()
{
    m_lookForTrustedDeviceAgent->stop();

    quint8 farDevices {};

    for (const auto &discoveredDevice : m_lookForTrustedDeviceAgent->discoveredDevices()) {
        for (const auto &trustedDevice : m_trustedDevices) {
            if (discoveredDevice.address() != trustedDevice.address()) {
                continue;
            }

            if (discoveredDevice.rssi() == 0) {
                ++farDevices;
            }
        }
    }

    if (farDevices >= m_trustedDevices.size()) {
        if (not m_screenLocker.isScreenLocked()) {
            emit lockScreen();
        }
    }

    m_lookForTrustedDeviceTimer.start();
}

void BluetoothListener::checkForTrustedDevice()
{
    m_lookForTrustedDeviceTimer.stop();
    m_lookForTrustedDeviceAgent->start();
    QTimer::singleShot(30'000, this, &BluetoothListener::checkForTrustedDeviceScanCompleted);
}
