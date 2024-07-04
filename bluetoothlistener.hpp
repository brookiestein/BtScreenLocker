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
public:
    explicit BluetoothListener(ScreenLocker &locker, QObject *parent = nullptr);
    ~BluetoothListener();
    void startDiscovery();
signals:
    void lockScreen(); /* When this signal is emitted, ScreenLocker::lockScreen() should be called. */
private slots:
    void hostModeStateChanged(QBluetoothLocalDevice::HostMode state);
    void discoverDevicesTimeout();
    void checkForTrustedDeviceScanCompleted();
    void checkForTrustedDevice();
};

#endif // BLUETOOTHLISTENER_HPP
