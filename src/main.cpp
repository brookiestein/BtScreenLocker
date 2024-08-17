#include <csignal>
#include <map>

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDBusConnection>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QLocale>
#include <QSharedPointer>
#include <QStandardPaths>
#include <QTranslator>
#include <QVector>

#include "listener.hpp"
#include "logger.hpp"
#include "screenlocker.hpp"

void setupSignals(const QVector<int> sigs);
void signalHandler(int signal);
QList<QCommandLineOption> commandLineOptions(const char *name);
QTranslator *setAppLanguage(std::map<QString, Logger::TYPE> &logMessages, const QString &language);
int enableAutostart(const QCommandLineParser &parser, Logger &logger, const char *name);
void registerDBusService(Listener &listener, Logger &logger);

int main(int argc, char *argv[])
{
    QApplication::setApplicationName(PROJECT_NAME);
    QApplication::setApplicationVersion(PROJECT_VERSION);
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    setupSignals({ SIGINT, SIGHUP, SIGTERM, SIGQUIT });

    QCommandLineParser parser;
    parser.setApplicationDescription(PROJECT_DESCRIPTION);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions(commandLineOptions(argv[0]));
    parser.process(a);

    std::map<QString, Logger::TYPE> logMessages;
    auto translator = QSharedPointer<QTranslator>(setAppLanguage(logMessages, parser.value("language")));
    a.installTranslator(translator.data());

    auto &logger = Logger::instance();
    logger.setLogFile(parser.value("filename"));

#ifdef QT_DEBUG
    logger.setVerbose();
#else
    if (parser.isSet("verbose")) {
        logger.setVerbose();
    }

    if (parser.isSet("debug")) {
        logger.setDebug();
    }
#endif

    for (const auto &[message, type] : logMessages) {
        logger.log(message, Q_FUNC_INFO, type);
    }

    if (parser.isSet("kill")) {
        logger.log(QObject::tr("Sending kill signal..."), Q_FUNC_INFO);
        QDBusConnection::sessionBus()
            .send(QDBusMessage::createMethodCall(SERVICE_NAME, "/Listen", "", "kill"));
        return 0;
    }

    if (parser.isSet("pause")) {
        logger.log(QObject::tr("Sending pause signal..."), Q_FUNC_INFO);
        QDBusConnection::sessionBus()
            .send(QDBusMessage::createMethodCall(SERVICE_NAME, "/Listen", "", "pause"));
        return 0;
    }

    if (parser.isSet("resume")) {
        logger.log(QObject::tr("Sending resume signal..."), Q_FUNC_INFO);
        QDBusConnection::sessionBus()
            .send(QDBusMessage::createMethodCall(SERVICE_NAME, "/Listen", "", "resume"));
        return 0;
    }

    if (parser.isSet("scan-again")) {
        logger.log(QObject::tr("Sending startAgain signal..."), Q_FUNC_INFO);
        QDBusConnection::sessionBus()
            .send(QDBusMessage::createMethodCall(SERVICE_NAME, "/Listen", "", "scanAgain"));
        return 0;
    }

    if (parser.isSet("autostart")) {
        return enableAutostart(parser, logger, argv[0]);
    }

    QString timeToRestart = parser.value("time-to-restart");
    bool autorestart = parser.isSet("auto-restart");
    bool ok {false};
    int seconds = timeToRestart.toInt(&ok);

    if (autorestart) {
        if (not timeToRestart.isEmpty() and not ok) {
            auto message = QObject::tr("%1 is not a valid value for 'seconds'.").arg(timeToRestart);
            QMessageBox::critical(nullptr, QObject::tr("Error"), message);
            logger.log(message, Q_FUNC_INFO, Logger::TYPE::FATAL);
        }

        if (timeToRestart.isEmpty()) {
            seconds = 10; /* Default to 10 seconds if user didn't provide it. */
        }
    }

    ScreenLocker locker;
    Listener listener(locker, autorestart, seconds);
    a.connect(&listener, &Listener::lockScreen, &locker, &ScreenLocker::lockScreen);
    a.connect(&listener, &Listener::quit, &a, &QApplication::quit, Qt::QueuedConnection);
    /* When screen is locked, no scan is done.
     * Connect to this signal to start scanning when screen is unlocked again.
     */
    a.connect(&locker, &ScreenLocker::activeChanged, &listener, &Listener::activeChanged);

    if (parser.isSet("list-devices")) {
        listener.listDevices();
        a.quit();
    }

    if (parser.isSet("remove-device")) {
        listener.removeDevice(parser.value("remove-device"));
        a.quit();
    }

    if (parser.isSet("discover")) {
        listener.startDiscovery();
    } else {
        listener.start();
    }

    registerDBusService(listener, logger);

    return a.exec();
}

void setupSignals(const QVector<int> sigs)
{
    for (const int sig : sigs) {
        signal(sig, signalHandler);
    }
}

void signalHandler(int signal)
{
    qInfo() << "Exiting because of signal:" << signal;
    QApplication::exit(signal);
}

QList<QCommandLineOption> commandLineOptions(const char *name)
{
    QList<QCommandLineOption> options;

    options.append(QCommandLineOption(QStringList() << "A" << "auto-restart",
        QObject::tr("Allow %1 to restart itself when your Bluetooth device "
                    "becomes unavailable and availabe again, e.g. when your machine suspends. "
                    "Default is 10 seconds.").arg(name))
    );

    options.append(QCommandLineOption(QStringList() << "a" << "autostart",
        QObject::tr("Register %1 to automatically start on boot.\n"
                    "(Will always autostart with passed parameters).").arg(name))
    );

    options.append(QCommandLineOption(QStringList() << "D" << "debug",
        QObject::tr("Enable debug log (implicitly enables verbose mode)."))
    );

    options.append(QCommandLineOption(QStringList() << "d" << "discover",
        QObject::tr("Discover new Bluetooth devices in order to add them to the trusted list."))
    );

    options.append(QCommandLineOption(QStringList() << "f" << "filename",
        QObject::tr("Where to save log. (Always verbose, debug depends on --debug)."), "filename")
    );

    options.append(QCommandLineOption(QStringList() << "k" << "kill",
        QObject::tr("End an existing %1 instance.").arg(name))
    );

    options.append(QCommandLineOption(QStringList() << "l" << "language",
        QObject::tr("Set language. (Available: English (default), and Spanish"), "language")
    );

    options.append(QCommandLineOption(QStringList() << "L" << "list-devices",
        QObject::tr("List trusted devices.")));

    options.append(QCommandLineOption(QStringList() << "p" << "pause",
        QObject::tr("Pause an already running %1 instance.").arg(name))
    );

    options.append(QCommandLineOption(QStringList() << "R" << "remove-device",
        QObject::tr("Remove #th trusted device. Use -L to see all trusted devices."), "#"));

    options.append(QCommandLineOption(QStringList() << "r" << "resume",
        QObject::tr("Resume an already running and paused %1 instance.").arg(name))
    );

    options.append(QCommandLineOption(QStringList() << "s" << "scan-again",
        QObject::tr("Same as --discover, but for an already running %1 instance.").arg(name))
    );

    options.append(QCommandLineOption(QStringList() << "t" << "time-to-restart",
        QObject::tr("Time to wait before auto-restarting %1. (Only useful when combined with -A)").arg(name),
        "seconds")
    );

    options.append(QCommandLineOption(QStringList() << "V" << "verbose",
        QObject::tr("Enable verbose log."))
    );

    return options;
}

/* Sets language or leave English as default. */
QTranslator *setAppLanguage(std::map<QString, Logger::TYPE> &logMessages, const QString &language)
{
    if (not language.isEmpty()) {
        logMessages[QObject::tr("Trying to set program language to: %1...").arg(language)] = Logger::INFO;
    }

    QTranslator *translator = new QTranslator();
    if (language.isEmpty()) {
        const QStringList uiLanguages = QLocale::system().uiLanguages();
        for (const QString &locale : uiLanguages) {
            const QString baseName = "BtScreenLocker_" + QLocale(locale).name();
            if (translator->load(QLocale(), ":/translations/" + baseName)) {
                break;
            }
        }
    } else if (language == "es" or language == "spanish" or language == "español") {
        translator->load(QLocale(), ":/translations/BtScreenLocker_es_US.qm");
    } else if (language == "en" or language == "english") {
        logMessages[QObject::tr("Language refers to default language: English.")] = Logger::INFO;
    } else {
        logMessages[QObject::tr("Language: %1 currently not supported.").arg(language)] = Logger::ERROR;
    }

    if (translator->language().isEmpty()) {
        logMessages[QObject::tr("Using default language: English.")] = Logger::INFO;
    } else {
        logMessages[QObject::tr("Using language: %1.").arg(translator->language())] = Logger::INFO;
    }

    return translator;
}

int enableAutostart(const QCommandLineParser &parser, Logger &logger, const char *name)
{
    QString filename(":/resources/BtScreenLocker.desktop");
    QFile file(filename);
    if (not file.open(QIODevice::ReadOnly)) {
        logger.log(QObject::tr("Error while loading the autostart file. Can't continue."),
                   Q_FUNC_INFO, Logger::ERROR);
        return 1;
    }

    QStringList args;
    if (parser.isSet("auto-restart")) {
        args << "-A";
    }

    if (parser.isSet("time-to-restart")) {
        args << "-t" << parser.value("time-to-restart");
    }

    if (parser.isSet("debug")) {
        args << "-D";
    }

    if (parser.isSet("filename")) {
        args << QString("-f %1").arg(parser.value("filename"));
    }

    if (parser.isSet("language")) {
        args << QString("-l %1").arg(parser.value("language"));
    }

    if (parser.isSet("verbose")) {
        args << "-V";
    }

    auto contents = QString::fromUtf8(file.readAll());
    QString a;
    for (const auto &s : args) {
        a += QString(" %1").arg(s);
    }

    contents.replace("/bin/BtScreenLocker", "/bin/BtScreenLocker" + a);

    filename = QString("%1%2%3%4%5")
                   .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation),
                        QDir::separator(),
                        "autostart",
                        QDir::separator(),
                        AUTOSTART_FILE);

    if (QFile::exists(filename)) {
        logger.log(
            QObject::tr("Warning! Overwriting old autostart configuration."),
            Q_FUNC_INFO,
            Logger::WARNING
        );
    }

    file.close();
    file.setFileName(filename);
    if (not file.open(QIODevice::WriteOnly)) {
        logger.log(
            QObject::tr("Error while opening autostart file. Can't continue.\nError: %1").arg(file.errorString()),
            Q_FUNC_INFO, Logger::ERROR
        );
        return 1;
    }

    file.write(QByteArray::fromStdString(contents.toStdString()));
    if (not file.flush()) {
        logger.log(
            QObject::tr("Error while writing autostart file.\nError: %1").arg(file.errorString()),
            Q_FUNC_INFO
        );
        return 1;
    }

    logger.log(QObject::tr("%1 will autostart on boot!").arg(name), Q_FUNC_INFO);
    return 0;
}

void registerDBusService(Listener &listener, Logger &logger)
{
    auto connection = QDBusConnection::sessionBus();
    if (not connection.registerService(SERVICE_NAME)) {
            logger.log(
            QObject::tr("Couldn't register D-Bus service. Won't be able to respond to IPC messages.\n"
                       "Error message: %1").arg(connection.lastError().message()),
            Q_FUNC_INFO, Logger::ERROR);
        return;
    }

    connection.registerObject("/Listen", &listener, QDBusConnection::ExportScriptableSlots);
}
