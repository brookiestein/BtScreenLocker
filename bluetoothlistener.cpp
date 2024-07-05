#include "bluetoothlistener.hpp"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

#include "devicechooser.hpp"
#include "logger.hpp"

BluetoothListener::BluetoothListener(ScreenLocker &screenLocker, bool verbose, bool debug, QObject *parent)
    : QObject{parent}
    , m_screenLocker(screenLocker)
    , m_debug(debug)
    , m_verbose(verbose)
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

        m_lookForTrustedDeviceTimer.start();
    }
}

BluetoothListener::~BluetoothListener()
{
    m_settings->endGroup();
}

void BluetoothListener::startDiscovery()
{
    /* If constructor started it, but in main.cpp user started discovery. */
    m_lookForTrustedDeviceTimer.stop();
    Logger::log(tr("Discoverying devices..."), Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);

    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::DiscoveryMethod::ClassicMethod);
    m_deviceDiscoverTimer.start();

    QMessageBox::information(nullptr, tr("Discoverying devices"),
                             tr("Please wait up to 30 seconds, I'll show you the devices I discover."));
}

void BluetoothListener::setDebug()
{
    m_debug = true;
}

void BluetoothListener::setVerbose()
{
    m_verbose = true;
}

void BluetoothListener::hostModeStateChanged(QBluetoothLocalDevice::HostMode state)
{
    bool poweredOff = state == QBluetoothLocalDevice::HostPoweredOff;
    Logger::log(tr("Bluetooth device changed its status."),
                poweredOff ? Logger::WARNING : Logger::INFO,
                m_verbose, m_debug, Q_FUNC_INFO);

    if (poweredOff) {
        auto reply = QMessageBox::question(nullptr,
                                           tr("Bluetooth is off"),
                                           tr("For this program to work, you have to have bluetooth on\n"
                                              "Would you like to turn it on?"));
        if (reply != QMessageBox::Yes) {
            Logger::log(
                tr("Exiting because there's nothing else to do."),
                Logger::WARNING, m_verbose, m_debug,
                Q_FUNC_INFO
            );

            QMessageBox::critical(nullptr, tr("Quitting"), tr("Nothing else to do."));
            std::exit(1);
        }

        m_localDevice.powerOn();
    }
}

void BluetoothListener::discoverDevicesTimeout()
{
    Logger::log(tr("Device discovery finished."), Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
    m_deviceDiscoverTimer.stop();
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
        Logger::log(tr("Adding device %1 to the trusted devices list...").arg(name),
                    Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
        m_settings->setValue(name, address);
    }

    Logger::log(tr("Checking whether trusted devices are near..."),
                Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
    m_lookForTrustedDeviceTimer.start();
}

void BluetoothListener::checkForTrustedDeviceScanCompleted()
{
    Logger::log(tr("Scan for trusted devices finished."), Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
    m_lookForTrustedDeviceAgent->stop();

    QStringList nearDevices;
    quint8 farDevices {};

    for (const auto &discoveredDevice : m_lookForTrustedDeviceAgent->discoveredDevices()) {
        for (const auto &trustedDevice : m_trustedDevices) {
            if (discoveredDevice.address() != trustedDevice.address()) {
                continue;
            }

            if (discoveredDevice.rssi() == 0) {
                ++farDevices;
            } else {
                nearDevices << discoveredDevice.name();
            }
        }
    }

    if (farDevices >= m_trustedDevices.size()) {
        if (not m_screenLocker.isScreenLocked()) {
            Logger::log(
                tr("Locking screen because all trusted devices are far way."),
                Logger::INFO,
                m_verbose, m_debug, Q_FUNC_INFO);
            emit lockScreen();
        }

        return;
    }

    QString devices;
    for (int i {}; i < nearDevices.size(); ++i) {
        devices += nearDevices[i];
        devices += i < nearDevices.size() - 1 ? ", " : "";
    }

    Logger::log(
        tr("Not locking screen because the following trusted devices are near: %1").arg(devices),
        Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO
    );

    m_lookForTrustedDeviceTimer.start();
}

void BluetoothListener::checkForTrustedDevice()
{
    Logger::log(tr("Checking whether trusted devices are near..."),
                Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
    m_lookForTrustedDeviceTimer.stop();
    m_lookForTrustedDeviceAgent->start();
    QTimer::singleShot(30'000, this, &BluetoothListener::checkForTrustedDeviceScanCompleted);
}

void BluetoothListener::screenActive()
{
    m_lookForTrustedDeviceTimer.start();
}
