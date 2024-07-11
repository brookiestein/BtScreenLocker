#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QObject>

class Logger : public QObject
{
    Q_OBJECT
    bool m_verbose;
    bool m_debug;
    QString m_filename;
public:
    enum TYPE { INFO = 0, WARNING, ERROR, FATAL };
    Logger(bool verbose = false, bool debug = false, const QString &filename = QString());
    void setVerbose();
    void setDebug();
    void setLogFile(const QString &filename);
    void log(const QString &message,
                    const QString &function = QString(),
                    TYPE type = INFO
                    );
};

#endif // LOGGER_HPP
