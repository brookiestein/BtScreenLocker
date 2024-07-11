#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDBusConnection>
#include <QDebug>
#include <QLocale>
#include <QTranslator>

#include "listener.hpp"
#include "logger.hpp"
#include "screenlocker.hpp"

QList<QCommandLineOption> commandLineOptions(const char *name);
void setAppLanguage(QApplication &a, const QString &language, Logger &logger);
void registerDBusService(Listener &listener, Logger &logger);
void usage(const char *programName);

int main(int argc, char *argv[])
{
    QApplication::setApplicationName(PROJECT_NAME);
    QApplication::setApplicationVersion(PROJECT_VERSION);
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QCommandLineParser parser;
    parser.setApplicationDescription(PROJECT_DESCRIPTION);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions(commandLineOptions(argv[0]));
    parser.process(a);

    Logger logger;
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

    setAppLanguage(a, parser.value("language").toLower(), logger);

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

    ScreenLocker locker;
    Listener listener(locker, logger);
    a.connect(&listener, &Listener::lockScreen, &locker, &ScreenLocker::lockScreen);
    a.connect(&listener, &Listener::quit, &a, &QApplication::quit);
    /* When screen is locked, no scan is done.
     * Connect to this signal to start scanning when screen is unlocked again.
     */
    a.connect(&locker, &ScreenLocker::activeChanged, &listener, &Listener::activeChanged);

    if (parser.isSet("discover")) {
        listener.startDiscovery();
    } else {
        listener.start();
    }

    registerDBusService(listener, logger);

    return a.exec();
}

QList<QCommandLineOption> commandLineOptions(const char *name)
{
    QList<QCommandLineOption> options;
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

    options.append(QCommandLineOption(QStringList() << "p" << "pause",
        QObject::tr("Pause an already running %1 instance.").arg(name))
    );

    options.append(QCommandLineOption(QStringList() << "r" << "resume",
        QObject::tr("Resume an already running and paused %1 instance.").arg(name))
    );

    options.append(QCommandLineOption(QStringList() << "s" << "scan-again",
        QObject::tr("Same as --discover, but for an already running %1 instance.").arg(name))
    );

    options.append(QCommandLineOption(QStringList() << "V" << "verbose",
        QObject::tr("Enable verbose log."))
    );

    return options;
}

/* Sets language or leave English as default. */
void setAppLanguage(QApplication &a, const QString &language, Logger &logger)
{
    QTranslator translator;
    if (language.isEmpty()) {
        const QStringList uiLanguages = QLocale::system().uiLanguages();
        for (const QString &locale : uiLanguages) {
            const QString baseName = "BtScreenLocker_" + QLocale(locale).name();
            if (translator.load(":/translations/" + baseName)) {
                a.installTranslator(&translator);
                break;
            }
        }
    } else if (language == "es" or language == "spanish" or language == "español") {
        if (translator.load(":/translations/BtScreenLocker_es_US")) {
            a.installTranslator(&translator);
        }
    } else if (language == "en" or language == "english") {
        logger.log(QObject::tr("Language refers to default language: English."), Q_FUNC_INFO);
    } else {
        logger.log(QObject::tr("Language: %1 currently not supported.").arg(language), Q_FUNC_INFO);
    }

    if (translator.language().isEmpty()) {
        logger.log(QObject::tr("Using default language: English."), Q_FUNC_INFO);
    } else {
        logger.log(QObject::tr("Using language: %1.").arg(translator.language()), Q_FUNC_INFO);
    }
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
