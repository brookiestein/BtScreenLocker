#ifndef SCREENLOCKER_HPP
#define SCREENLOCKER_HPP

#include <QDBusConnection>
#include <QDBusMessage>
#include <QObject>

class ScreenLocker : public QObject
{
    Q_OBJECT
    bool m_screenLocked;
    QDBusConnection m_connection;
public:
    explicit ScreenLocker(QObject *parent = nullptr);
    bool isScreenLocked();
public slots:
    void lockScreen();
private slots:
    void screenChanged(const QDBusMessage &message);
};

#endif // SCREENLOCKER_HPP
