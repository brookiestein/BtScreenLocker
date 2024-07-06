#ifndef BLUETOOTHLISTENER_HPP
#define BLUETOOTHLISTENER_HPP

#include <QBluetoothDeviceInfo>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QObject>
#include <QSettings>
#include <QTimer>

#include "screenlocker.hpp"

class BluetoothListener : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", SERVICE_NAME)

    QDBusConnection m_dbusConnection;
    QSettings *m_settings;
    QBluetoothLocalDevice m_localDevice;
    QList<QBluetoothDeviceInfo> m_trustedDevices;
    QTimer m_deviceDiscoverTimer;
    QTimer m_lookForTrustedDeviceTimer;
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;
    ScreenLocker &m_screenLocker;
    bool m_debug;
    bool m_verbose;
    bool m_stopped;

public:
    explicit BluetoothListener(ScreenLocker &locker, bool verbose = false, bool debug = false, QObject *parent = nullptr);
    ~BluetoothListener();
    void start();
    void startDiscovery();
    void setDebug();
    void setVerbose();
signals:
    void lockScreen(); /* When this signal is emitted, ScreenLocker::lockScreen() should be called. */
private slots:
    void hostModeStateChanged(QBluetoothLocalDevice::HostMode state);
    void discoverDevicesTimeout();
    void checkForTrustedDeviceScanCompleted();
    void checkForTrustedDevice();
public slots:
    void screenActive();
    Q_SCRIPTABLE void pause(); /* To pause looking for trusted devices and locking the screen without closing the program. */
    Q_SCRIPTABLE void resume(); /* Opposite */
    Q_SCRIPTABLE void scanAgain();
};

#endif // BLUETOOTHLISTENER_HPP
