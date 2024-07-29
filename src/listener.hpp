#ifndef BLUETOOTHLISTENER_HPP
#define BLUETOOTHLISTENER_HPP

#include <QBluetoothDeviceInfo>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QObject>
#include <QSettings>
#include <QSharedPointer>
#include <QTimer>
#include <QThread>

#include "logger.hpp"
#include "screenlocker.hpp"

class Listener : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", SERVICE_NAME)

    QDBusConnection m_dbusConnection;
    QSharedPointer<QSettings> m_settings;
    QBluetoothLocalDevice m_localDevice;
    QVector<QBluetoothDeviceInfo> m_trustedDevices;
    QTimer m_deviceDiscoverTimer;
    QTimer m_lookForTrustedDeviceTimer;
    QSharedPointer<QBluetoothDeviceDiscoveryAgent> m_discoveryAgent;
    QSharedPointer<QThread> connectionThread;
    ScreenLocker &m_screenLocker;
    Logger &m_logger;
    bool m_stopped;
    bool m_restarting;
    bool m_autorestart;
    int m_seconds;

    void restart();
public:
    explicit Listener(ScreenLocker &locker, bool autorestart = false, int seconds = -1, QObject *parent = nullptr);
    ~Listener();
    void start();
    void startDiscovery();
signals:
    void lockScreen(); /* When this signal is emitted, ScreenLocker::lockScreen() should be called. */
    void quit();
private slots:
    void hostModeStateChanged(QBluetoothLocalDevice::HostMode state);
    void discoverDevicesTimeout();
    void checkForTrustedDevice();
public slots:
    void activeChanged(bool locked);
    Q_SCRIPTABLE void pause(); /* To pause looking for trusted devices and locking the screen without closing the program. */
    Q_SCRIPTABLE void resume(); /* Opposite */
    Q_SCRIPTABLE void scanAgain();
    Q_SCRIPTABLE void kill();
};

#endif // BLUETOOTHLISTENER_HPP
