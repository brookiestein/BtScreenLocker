#include "screenlocker.hpp"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>

ScreenLocker::ScreenLocker(QObject *parent)
    : QObject{parent}
{

}

void ScreenLocker::lockScreen()
{
    auto message = QDBusMessage::createMethodCall(
        "org.freedesktop.ScreenSaver",
        "/ScreenSaver",
        "",
        "Lock"
    );

    if (QDBusConnection::sessionBus().send(message)) {
        qDebug() << "Message sent!";
    } else {
        qFatal("Message couldn't be sent!");
    }
}
