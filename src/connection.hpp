#ifndef BLUETOOTHCONNECTION_HPP
#define BLUETOOTHCONNECTION_HPP

#include <QBluetoothDeviceInfo>
#include <QObject>

#include "logger.hpp"

class Connection : public QObject
{
    Q_OBJECT
    const QVector<QBluetoothDeviceInfo> &m_trustedDevices;
    QVector<QBluetoothDeviceInfo> m_connectedDevices;
    Logger &m_logger;
public:
    Connection(const QVector<QBluetoothDeviceInfo> &trustedDevices, Logger &logger, QObject *parent = nullptr);
    void connect();
    QVector<QBluetoothDeviceInfo> connectedDevices();
};

#endif // BLUETOOTHCONNECTION_HPP
