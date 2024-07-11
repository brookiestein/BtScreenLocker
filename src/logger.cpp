#include "logger.hpp"

#include <QDateTime>
#include <QDebug>
#include <QFile>

Logger::Logger(bool verbose, bool debug, const QString &filename)
    : m_verbose(verbose)
    , m_debug(debug)
    , m_filename(filename)
{
}

void Logger::setVerbose()
{
    m_verbose = true;
}

void Logger::setDebug()
{
    m_debug = true;
}

void Logger::setLogFile(const QString &filename)
{
    m_filename = filename;
}

void Logger::log(const QString &message, const QString &function, TYPE type)
{
    auto formattedMessage = QString("%1: ").arg(QDateTime::currentDateTime().toString(Qt::ISODate));
    if (m_debug) {
        formattedMessage += function + ": ";
    }

    formattedMessage += message;

    if (not m_filename.isEmpty()) {
        QFile file(m_filename);
        if (file.open(QIODevice::Append)) {
            file.write(QByteArray::fromStdString((formattedMessage + "\n").toStdString()));
        } else {
            qCritical().noquote() << QDateTime::currentDateTime().toString(Qt::ISODate)
                                  << "Couldn't open file:" << m_filename
                                  << "won't be able to write log to that file.";
        }
    }

    switch (type)
    {
    case INFO:
        if (m_verbose or m_debug) {
            qInfo().noquote() << formattedMessage;
        }
        break;
    case WARNING:
        if (m_verbose or m_debug) {
            qWarning().noquote() << formattedMessage;
        }
        break;
    case ERROR:
        qCritical().noquote() << formattedMessage;
        break;
    case FATAL:
        qFatal("%s\n", formattedMessage.toStdString().c_str());
        break;
    }
}
