#ifndef SCREENLOCKER_HPP
#define SCREENLOCKER_HPP

#include <QObject>

class ScreenLocker : public QObject
{
    Q_OBJECT
public:
    explicit ScreenLocker(QObject *parent = nullptr);
public slots:
    void lockScreen();
};

#endif // SCREENLOCKER_HPP
