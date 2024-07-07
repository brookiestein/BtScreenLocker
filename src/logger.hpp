#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QObject>

class Logger : public QObject
{
    Q_OBJECT
public:
    Logger() = delete;
    enum TYPE { INFO = 0, WARNING, ERROR, FATAL };
    static void log(const QString &message,
                    TYPE type = INFO,
                    bool verbose = true,
                    bool debug = false,
                    const QString &function = QString()
                    );
};

#endif // LOGGER_HPP
