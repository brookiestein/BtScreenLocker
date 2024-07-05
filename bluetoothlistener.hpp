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

    QSettings *m_settings;
    QBluetoothLocalDevice m_localDevice;
    QList<QBluetoothDeviceInfo> m_trustedDevices;
    QTimer m_deviceDiscoverTimer;
    QTimer m_lookForTrustedDeviceTimer;
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;
    QBluetoothDeviceDiscoveryAgent *m_lookForTrustedDeviceAgent;
    ScreenLocker &m_screenLocker;
    bool m_debug;
    bool m_verbose;
public:
    explicit BluetoothListener(ScreenLocker &locker, bool verbose = false, bool debug = false, QObject *parent = nullptr);
    ~BluetoothListener();
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
};

#endif // BLUETOOTHLISTENER_HPP
