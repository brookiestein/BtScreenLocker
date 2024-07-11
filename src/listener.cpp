#include "listener.hpp"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

#include "connection.hpp"
#include "devicechooser.hpp"
#include "logger.hpp"

Listener::Listener(ScreenLocker &screenLocker, bool verbose, bool debug, QObject *parent)
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

    m_discoveryAgent = QSharedPointer<QBluetoothDeviceDiscoveryAgent>(new QBluetoothDeviceDiscoveryAgent(this));

    m_deviceDiscoverTimer.setInterval(30'000); /* Discover devices for 30 seconds. */
    m_lookForTrustedDeviceTimer.setInterval(30'000); /* Check if trusted device is near every 10 seconds. */

    auto filename = QString("%1%2%3%4%5.ini")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation),
                             QDir::separator(),
                             PROJECT_NAME,
                             QDir::separator(),
                             PROJECT_NAME
                             );

    m_settings = QSharedPointer<QSettings>(new QSettings(filename, QSettings::IniFormat, this));
    m_settings->beginGroup("TrustedDevices");

    connect(&m_deviceDiscoverTimer, &QTimer::timeout, this, &Listener::discoverDevicesTimeout);
    connect(&m_lookForTrustedDeviceTimer, &QTimer::timeout, this, &Listener::checkForTrustedDevice);
    connect(&m_localDevice, &QBluetoothLocalDevice::hostModeStateChanged, this, &Listener::hostModeStateChanged);
}

Listener::~Listener()
{
    m_settings->endGroup();
}

void Listener::start()
{
    if (m_settings->allKeys().isEmpty()) {
        startDiscovery();
        return;
    }

    m_trustedDevices.clear();
    for (const auto &deviceName : m_settings->allKeys()) {
        auto address = QBluetoothAddress(m_settings->value(deviceName).toString());
        auto deviceInfo = QBluetoothDeviceInfo(address, deviceName, 0);

        m_trustedDevices.push_back(deviceInfo);
    }

    m_lookForTrustedDeviceTimer.start();
}

void Listener::startDiscovery()
{
    /* If constructor started it, but in main.cpp user started discovery. */
    m_lookForTrustedDeviceTimer.stop();
    Logger::log(tr("Discoverying devices..."), Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);

    m_discoveryAgent->start();
    m_deviceDiscoverTimer.start();

    QMessageBox::information(nullptr, tr("Discoverying devices"),
                             tr("Please wait up to 30 seconds, I'll show you the devices I discover."));
}

void Listener::setDebug()
{
    m_debug = true;
}

void Listener::setVerbose()
{
    m_verbose = true;
}

void Listener::hostModeStateChanged(QBluetoothLocalDevice::HostMode state)
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

void Listener::discoverDevicesTimeout()
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

void Listener::checkForTrustedDevice()
{
    Logger::log(tr("Checking whether trusted devices are near..."),
                Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);

    m_lookForTrustedDeviceTimer.stop();

    connectionThread.clear();
    connectionThread = QSharedPointer<QThread>(QThread::create([this] () {
        Connection connection(m_trustedDevices, m_verbose, m_debug);
        connection.connect();
        auto connectedDevices = connection.connectedDevices();

        if (connectedDevices.empty()) {
            Logger::log(
                tr("Locking screen because all trusted devices are far way."),
                Logger::INFO,
                m_verbose, m_debug, Q_FUNC_INFO);
            emit lockScreen();

            return;
        }

        QString devices;
        for (int i {}; i < connectedDevices.size(); ++i) {
            const auto &deviceInfo = connectedDevices[i];
            devices += QString("%1. %2 - %3%4")
                           .arg(QString::number(i + 1),
                                deviceInfo.address().toString(),
                                deviceInfo.name(),
                                (i < connectedDevices.size() - 1 ? "\n" : "")
                        );
        }

        Logger::log(
            tr("Not locking screen because the following trusted devices are near:\n%1").arg(devices),
            Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO
            );
    }));

    connect(connectionThread.data(), SIGNAL(finished()), &m_lookForTrustedDeviceTimer, SLOT(start()));

    connectionThread->start();
}

void Listener::activeChanged(bool locked)
{
    QString message;

    if (locked) {
        message = tr("Not looking for trusted devices because screen is locked.");
        m_lookForTrustedDeviceTimer.stop();
    } else {
        message = tr("Screen active. Looking for trusted devices again.");
        m_lookForTrustedDeviceTimer.start();
    }

    Logger::log(message, Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
}

void Listener::pause()
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

void Listener::resume()
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

void Listener::scanAgain()
{
    Logger::log(tr("Discoverying device started by D-Bus signal telling me so."),
                Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
    startDiscovery();
}

void Listener::kill()
{
    Logger::log(tr("Quitting because of D-Bus signal telling me so..."),
                Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO);
    emit quit();
}
