#ifndef BLUETOOTHCONNECTION_HPP
#define BLUETOOTHCONNECTION_HPP

#include <QBluetoothDeviceInfo>
#include <QObject>

class Connection : public QObject
{
    Q_OBJECT
    const QVector<QBluetoothDeviceInfo> &m_trustedDevices;
    QVector<QBluetoothDeviceInfo> m_connectedDevices;
    bool m_verbose;
    bool m_debug;
public:
    Connection(const QVector<QBluetoothDeviceInfo> &trustedDevices, bool verbose, bool debug, QObject *parent = nullptr);
    void connect();
    QVector<QBluetoothDeviceInfo> connectedDevices();
};

#endif // BLUETOOTHCONNECTION_HPP
