#include "bluetoothlistener.hpp"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

#include "devicechooser.hpp"

BluetoothListener::BluetoothListener(QObject *parent)
    : QObject{parent}
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

    connect(&m_localDevice, &QBluetoothLocalDevice::hostModeStateChanged, this, &BluetoothListener::hostModeStateChanged);

    m_deviceDiscoverTimer.setInterval(30'000); /* Discover devices for 30 seconds. */
    connect(&m_deviceDiscoverTimer, &QTimer::timeout, this, &BluetoothListener::discoverDevicesTimeout);

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
