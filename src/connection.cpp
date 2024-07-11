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

Connection::Connection(const QVector<QBluetoothDeviceInfo> &trustedDevices,
                       bool verbose, bool debug, QObject *parent)
    : QObject{parent}
    , m_trustedDevices(trustedDevices)
    , m_verbose(verbose)
    , m_debug(debug)
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

        Logger::log(
            tr("Trying to connect to Bluetooth device: %1 - %2...").arg(address.data(), name),
            Logger::INFO, m_verbose, m_debug, Q_FUNC_INFO
        );

        int sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (sock < 0) {
            Logger::log(tr("Couldn't allocate socket to connect to Bluetooth device: %1\n"
                           "Error: %2").arg(address.data(), std::strerror(errno)),
                        Logger::ERROR, m_verbose, m_debug, Q_FUNC_INFO
            );
            close(sock);
            break;
        }

        int status = ::connect(sock, (struct sockaddr *) &target, sizeof(target));
        if (status == 0) {
            close(sock);
            Logger::log(tr("Connected to Bluetooth device: %1 - %2!").arg(address.data(), name));
            m_connectedDevices.push_back(device);
            continue;
        }

        Logger::log(
            tr("Couldn't connect to Bluetooth device: %1 - %2.\n"
               "It may be far or has Bluetooth disabled.").arg(address.data(), name),
            Logger::WARNING, m_verbose, m_debug, Q_FUNC_INFO
        );
    }
}

QVector<QBluetoothDeviceInfo> Connection::connectedDevices()
{
    return m_connectedDevices;
}