#include "screenlocker.hpp"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QMessageBox>

ScreenLocker::ScreenLocker(QObject *parent)
    : QObject{parent}
    , m_screenLocked(false)
    , m_connection(QDBusConnection::sessionBus())
{
    /* When screen gets locked or unlocked, listen for this event
     * not to try to lock the screen again if bluetooth trusted devices go away.
     */
    m_connection.connect("org.freedesktop.ScreenSaver",
                         "/ScreenSaver",
                         "",
                         "ActiveChanged",
                         this,
                         SLOT(screenChanged(const QDBusMessage&)));
}

bool ScreenLocker::isScreenLocked()
{
    return m_screenLocked;
}

void ScreenLocker::lockScreen()
{
    auto message = QDBusMessage::createMethodCall(
        "org.freedesktop.ScreenSaver",
        "/ScreenSaver",
        "",
        "Lock"
    );

    if (not QDBusConnection::sessionBus().send(message)) {
        QMessageBox::critical(nullptr,
                              tr("Error"),
                              tr("Trusted devices are away, but screen couldn't be locked."));
    }
}

void ScreenLocker::screenChanged(const QDBusMessage &message)
{
    m_screenLocked = message.arguments().at(0).toString() == "true" ? true : false;
    emit activeChanged(m_screenLocked);
}

