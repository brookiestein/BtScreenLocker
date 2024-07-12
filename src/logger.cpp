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
    auto date = QDateTime::currentDateTime().toString(Qt::ISODate);
    QString formattedMessage {};
    if (m_debug) {
        formattedMessage += function + ": ";
    }

    formattedMessage += message;
    auto finalMessage = QString("%1: ").arg(date);
    switch (type)
    {
    case INFO:
        finalMessage += QString("INFO: %1").arg(formattedMessage);
        if (m_verbose or m_debug) {
            qInfo().noquote() << finalMessage;
        }
        break;
    case WARNING:
        finalMessage += QString("WARN: %1").arg(formattedMessage);
        qWarning().noquote() << finalMessage;
        break;
    case ERROR:
        finalMessage += QString("ERROR: %1").arg(formattedMessage);
        qCritical().noquote() << finalMessage;
        break;
    case FATAL:
        finalMessage += QString("FATAL: %1").arg(formattedMessage);
        qFatal("%s\n", finalMessage.toStdString().c_str());
        break;
    }

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
}
