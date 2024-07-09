#include "bluetoothlistener.hpp"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
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
    , m_dbusConnection(QDBusConnection::sessionBus())
    , m_screenLocker(screenLocker)
    , m_debug(debug)
    , m_verbose(verbose)
    , m_stopped(false)
{
#ifdef QT_DEBUG
    m_verbose = true;
#endif

    if (not m_localDevice.isValid()) {
        auto message = tr("There isn't a valid Bluetooth device on this machine. Can't do anything.");
        QMessageBox::critical(nullptr,
                              tr("Error"),
                              message);
        Logger::log(message, Logger::FATAL, true, debug, Q_FUNC_INFO);
    } else {
        m_localDevice.powerOn();
    }

    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    /*
     * Use this timer both for discoverying new devices and for looking for already trusted devices.
     * Pay attention where this object is connected to and disconnected from
     * checkForTrustedDeviceScanCompleted and discoverDevicesTimeout
     */
    m_deviceDiscoverTimer.setInterval(30'000); /* Discover devices for 30 seconds. */
    m_lookForTrustedDeviceTimer.setInterval(10'000); /* Check if trusted device is near every 10 seconds. */

    connect(&m_deviceDiscoverTimer, &QTimer::timeout, this, &BluetoothListener::checkForTrustedDeviceScanCompleted);
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
}

BluetoothListener::~BluetoothListener()
{
    m_settings->endGroup();
}

void BluetoothListener::start()
{
    if (m_settings->allKeys().isEmpty()) {
        startDiscovery();
        return;
    }

    m_trustedDevices.clear();
    for (const auto &deviceName : m_settings->allKeys()) {
        auto address = QBluetoothAddress(m_settings->value(deviceName).toString());
        auto deviceInfo = QSharedPointer<QBluetoothDeviceInfo>(new QBluetoothDeviceInfo(address, deviceName, 0));
        auto *socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

        m_trustedDevices[deviceInfo] = socket;
    }

    m_lookForTrustedDeviceTimer.start();
}

void BluetoothListener::startDiscovery()
{
    disconnect(&m_deviceDiscoverTimer, &QTimer::timeout, this, &BluetoothListener::checkForTrustedDeviceScanCompleted);
    connect(&m_deviceDiscoverTimer, &QTimer::timeout, this, &BluetoothListener::discoverDevicesTimeout);

    /* If constructor started it, but in main.cpp user started discovery. */
    m_lookForTrustedDeviceTimer.stop();
    Logger::log(tr("Discoverying devices..."), Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);

    m_discoveryAgent->start();
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
    if (state == QBluetoothLocalDevice::HostPoweredOff) {
        m_lookForTrustedDeviceTimer.stop();
        m_deviceDiscoverTimer.stop();
        auto message = tr("Bluetooth device: %1, %2 became unavailable.\n"
                          "Ending program execution because of this!\n\n"
                          "If at some point the device: %3 becomes available again, please re-run me.")
                           .arg(m_localDevice.name(), m_localDevice.address().toString(), m_localDevice.name());
        QMessageBox::critical(nullptr, tr("Error"), message);
        Logger::log(message, Logger::ERROR, true, m_debug, Q_FUNC_INFO);
        emit quit();
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

    disconnect(&m_deviceDiscoverTimer, &QTimer::timeout, this, &BluetoothListener::discoverDevicesTimeout);
    connect(&m_deviceDiscoverTimer, &QTimer::timeout, this, &BluetoothListener::checkForTrustedDeviceScanCompleted);

    for (const auto &device : m_discoveryAgent->discoveredDevices()) {
        if (device.rssi() == 0) { /* Paired devices will be shown even when they're not near. */
            continue;
        }

        chooser.addDevice(device.name(), device.address().toString(), QString::number(device.rssi()));
    }

    chooser.show();
    loop.exec();

    auto devicesInfo = chooser.selectedDevices();
    for (const auto &[name, address] : devicesInfo) {
        Logger::log(tr("Adding device %1 to the trusted devices list...").arg(name),
                    Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);

        m_settings->setValue(name, address);

    }

    if (m_settings->allKeys().isEmpty()) {
        auto reply = QMessageBox::question(nullptr,
                                           tr("Warning"),
                                           tr("You haven't added any device to the trusted list.\n"
                                              "Would you like to scan for devices again?"));
        if (reply == QMessageBox::Yes) {
            startDiscovery();
            return;
        }

        QMessageBox::information(nullptr,
                                 tr("Information"),
                                 tr("No looking for trusted devices because there isn't any.\n"
                                    "You can scan for devices again at any time by calling another instance "
                                    "of this program with --scan-again without closing this one.")
                                 );
        return;
    }

    if (not m_lookForTrustedDeviceTimer.isActive() and not m_deviceDiscoverTimer.isActive()) {
        start();
    }
}

void BluetoothListener::checkForTrustedDeviceScanCompleted()
{
    m_discoveryAgent->stop();

    QStringList nearDevices;
    quint8 farDevices {};

    for (const auto &discoveredDevice : m_discoveryAgent->discoveredDevices()) {
        for (auto it = m_trustedDevices.begin(); it != m_trustedDevices.end(); ++it) {
            if (discoveredDevice.address() != it->first->address()) {
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
        devices += QString("%1. %2%3")
                       .arg(QString::number(i + 1),
                            nearDevices[i],
                            i < nearDevices.size() - 1 ? ", " : "");
    }

    Logger::log(
        tr("Not locking screen because the following trusted devices are near:\n%1").arg(devices),
        Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO
    );

    m_lookForTrustedDeviceTimer.start();
}

void BluetoothListener::checkForTrustedDevice()
{
    Logger::log(tr("Checking whether trusted devices are near..."),
                Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);

    m_lookForTrustedDeviceTimer.stop();
    m_discoveryAgent->start();
    m_deviceDiscoverTimer.start();
}

void BluetoothListener::screenActive()
{
    m_lookForTrustedDeviceTimer.start();
}

void BluetoothListener::pause()
{
    if (m_stopped) {
        Logger::log(tr("Looking for trusted devices is already stopped, ignoring message."),
                    Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
        return;
    }

    Logger::log(tr("Looking for trusted devices stopped by D-Bus signal telling me so."),
                Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
    m_lookForTrustedDeviceTimer.stop();
    m_deviceDiscoverTimer.stop();
    m_discoveryAgent->stop();
    m_stopped = true;
}

void BluetoothListener::resume()
{
    if (not m_stopped) {
        Logger::log(tr("Looking for trusted devices isn't stopped, ignoring message."),
                    Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
        return;
    }

    Logger::log(tr("Looking for trusted devices again."),
                Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
    m_lookForTrustedDeviceTimer.start();
    m_stopped = false;
}

void BluetoothListener::scanAgain()
{
    Logger::log(tr("Discoverying device started by D-Bus signal telling me so."),
                Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
    startDiscovery();
}

void BluetoothListener::kill()
{
    Logger::log(tr("Quitting because of D-Bus signal telling me so..."),
                Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
    emit quit();
}
