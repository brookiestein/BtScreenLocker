#include "connection.hpp"
#include "logger.hpp"

#include <QBluetoothAddress>
#include <QSharedPointer>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(const QVector<QBluetoothDeviceInfo> &trustedDevices, QObject *parent)
    : QObject{parent}
    , m_trustedDevices(trustedDevices)
    , m_logger(Logger::instance())
{
}

void Connection::connect()
{
    const unsigned int length = 18; /* MAC Addresses are 17 characters long */
    QSharedPointer<char> address(new char[length]);
    struct sockaddr_rc target = {
        .rc_family = AF_BLUETOOTH,
        .rc_channel = 2
    };

    /* Try to connect to every single trusted device until one gets connected. */
    for (const auto &device : m_trustedDevices) {
        auto name = device.name();
        {
            auto addr = device.address().toString().toStdString();
            std::snprintf(address.data(), length, "%s", addr.c_str());
        }

        str2ba(address.data(), &target.rc_bdaddr);

        m_logger.log(
            tr("Trying to connect to Bluetooth device: %1 - %2...").arg(address.data(), name),
            Q_FUNC_INFO
        );

        int sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (sock < 0) {
            m_logger.log(tr("Couldn't allocate socket to connect to Bluetooth device: %1\n"
                           "Error: %2").arg(address.data(), std::strerror(errno)),
                        Q_FUNC_INFO, Logger::ERROR
            );
            close(sock);
            break;
        }

        int status = ::connect(sock, (struct sockaddr *) &target, sizeof(target));
        if (status == 0) {
            close(sock);
            m_logger.log(tr("Connected to Bluetooth device: %1 - %2!").arg(address.data(), name), Q_FUNC_INFO);
            m_connectedDevices.push_back(device);
            continue;
        }

        m_logger.log(
            tr("Couldn't connect to Bluetooth device: %1 - %2.\n"
               "It may be far or has Bluetooth disabled.").arg(address.data(), name),
            Q_FUNC_INFO, Logger::WARNING
        );
    }
}

QVector<QBluetoothDeviceInfo> Connection::connectedDevices()
{
    return m_connectedDevices;
}
