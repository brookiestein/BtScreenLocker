#ifndef BLUETOOTHLISTENER_HPP
#define BLUETOOTHLISTENER_HPP

#include <QBluetoothDeviceInfo>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QObject>
#include <QSettings>
#include <QTimer>

class BluetoothListener : public QObject
{
    Q_OBJECT

    QSettings *m_settings;
    QBluetoothLocalDevice m_localDevice;
    QList<QBluetoothDeviceInfo> m_trustedDevices;
    QTimer m_deviceDiscoverTimer;
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;
public:
    explicit BluetoothListener(QObject *parent = nullptr);
    ~BluetoothListener();
    void startDiscovery();
signals:
    void lock(); /* When this signal is emitted, ScreenLocker::lockScreen() should be called. */
private slots:
    void hostModeStateChanged(QBluetoothLocalDevice::HostMode state);
    void discoverDevicesTimeout();
};

#endif // BLUETOOTHLISTENER_HPP
