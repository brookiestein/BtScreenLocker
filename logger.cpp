#include "logger.hpp"

#include <QDateTime>
#include <QDebug>

void Logger::log(const QString &message, TYPE type, bool verbose, bool debug, const QString &function)
{
    auto formattedMessage = QString("%1: %2%3")
                                .arg(QDateTime::currentDateTime().toString(Qt::ISODate),
                                     debug ? function + ": " : "",
                                     message);

    switch (type)
    {
    case INFO:
        if (verbose or debug)
            qInfo().noquote() << formattedMessage;
        break;
    case WARNING:
        if (verbose or debug)
            qWarning().noquote() << formattedMessage;
    case ERROR:
        qCritical().noquote() << formattedMessage;
        break;
    case FATAL:
        qFatal("%s\n", formattedMessage.toStdString().c_str());
        break;
    }
}
