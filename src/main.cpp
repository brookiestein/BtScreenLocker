#include <QApplication>
#include <QDBusConnection>
#include <QDebug>
#include <QLocale>
#include <QTranslator>

#include <getopt.h>

#include "bluetoothlistener.hpp"
#include "logger.hpp"
#include "screenlocker.hpp"

void registerDBusService(BluetoothListener &listener, bool debug);
void usage(const char *programName);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "BtScreenLocker_" + QLocale(locale).name();
        if (translator.load(":/translations/" + baseName)) {
            Logger::log(QObject::tr("Using language: %1").arg(baseName));
            a.installTranslator(&translator);
            break;
        }
    }

    const struct option longOptions[] = {
        { "debug",      no_argument,    0,  'D' },
        { "discover",   no_argument,    0,  'd' },
        { "help",       no_argument,    0,  'h' },
        { "kill",       no_argument,    0,  'k' },
        { "pause",      no_argument,    0,  'p' },
        { "resume",     no_argument,    0,  'r' },
        { "scan-again", no_argument,    0,  's' },
        { "verbose",    no_argument,    0,  'v' },
        { "version",    no_argument,    0,  'V' },
        { nullptr, 0, nullptr, 0 }
    };

    bool debug {false};
    bool discover {false};
    bool verbose {false};
    int option = 0;
    while ((option = getopt_long(argc, argv, "DdhkprsvV", longOptions, nullptr)) >= 0) {
        switch (option)
        {
        case 'D':
            debug = true;
            break;
        case 'd':
            discover = true;
            break;
        case 'h':
            usage(argv[0]);
            return 0;
        case 'k':
            Logger::log(QObject::tr("Sending kill signal..."), Logger::INFO, verbose, debug, Q_FUNC_INFO);
            QDBusConnection::sessionBus()
                .send(QDBusMessage::createMethodCall(SERVICE_NAME, "/Listen", "", "kill"));
            return 0;
        case 'p':
            Logger::log(QObject::tr("Sending pause signal..."), Logger::INFO, verbose, debug, Q_FUNC_INFO);
            QDBusConnection::sessionBus()
                .send(QDBusMessage::createMethodCall(SERVICE_NAME, "/Listen", "", "pause"));
            return 0;
        case 'r':
            Logger::log(QObject::tr("Sending resume signal..."), Logger::INFO, verbose, debug, Q_FUNC_INFO);
            QDBusConnection::sessionBus()
                .send(QDBusMessage::createMethodCall(SERVICE_NAME, "/Listen", "", "resume"));
            return 0;
        case 's':
            Logger::log(QObject::tr("Sending startAgain signal..."), Logger::INFO, verbose, debug, Q_FUNC_INFO);
            QDBusConnection::sessionBus()
                .send(QDBusMessage::createMethodCall(SERVICE_NAME, "/Listen", "", "scanAgain"));
            return 0;
        case 'v':
            verbose = true;
            break;
        case 'V':
            qInfo() << PROJECT_NAME << "version" << PROJECT_VERSION;
            return 0;
        default:
            Logger::log(QObject::tr("Unknown option: %1").arg(optopt), Logger::ERROR, verbose, debug, Q_FUNC_INFO);
        }
    }

#ifdef QT_DEBUG
    verbose = true;
#endif

    if (translator.language().isEmpty()) {
        Logger::log(QObject::tr("Using default language: English."), Logger::INFO, verbose, debug, Q_FUNC_INFO);
    }

    ScreenLocker locker;
    BluetoothListener listener(locker);
    a.connect(&listener, &BluetoothListener::lockScreen, &locker, &ScreenLocker::lockScreen);
    a.connect(&listener, &BluetoothListener::quit, &a, &QApplication::quit);
    /* When screen is locked, no scan is done.
     * Connect to this signal to start scanning when screen is unlocked again.
     */
    a.connect(&locker, &ScreenLocker::screenActive, &listener, &BluetoothListener::screenActive);

    if (verbose) {
        listener.setVerbose();
    }

    if (debug) {
        listener.setDebug();
    }

    if (discover) {
        listener.startDiscovery();
    } else {
        listener.start();
    }

    registerDBusService(listener, debug);

    return a.exec();
}

void registerDBusService(BluetoothListener &listener, bool debug)
{
    auto connection = QDBusConnection::sessionBus();
    if (not connection.registerService(SERVICE_NAME)) {
        Logger::log(
            QObject::tr("Couldn't register D-Bus service. Won't be able to respond to IPC messages.\n"
                       "Error message: %1").arg(connection.lastError().message()),
            Logger::ERROR, true, debug, Q_FUNC_INFO);
        return;
    }

    connection.registerObject("/Listen", &listener, QDBusConnection::ExportScriptableSlots);
}

void usage(const char *programName)
{
    qInfo().noquote() << programName << "usage:\n";
    qInfo() << "--debug"
            << '\t' << '|' << "-D" << '\t'
            << "Enable debug log (implicitly enables verbose mode).";

    qInfo() << "--discover"
            << '\t' << '|' << "-d" << '\t'
            << "Discover new Bluetooth devices in order to add them to the trusted list.";

    qInfo() << "--help"
            << "\t\t" << '|' << "-h" << '\t'
            << "Show this help.";

    qInfo().noquote() << "--kill"
            << "\t\t" << '|' << "-k" << '\t'
            << "End an existing" << programName << "instance.";

    qInfo().noquote() << "--pause"
                      << '\t' << '|' << "-p" << '\t'
                      << "Pause an already running" << programName << "instance.";

    qInfo().noquote() << "--resume" << '\t' << '|' << "-r" << '\t'
                      << "Resume an already running and paused" << programName << "instance.";

    qInfo().noquote() << "--scan-again"
            << '\t' << '|' << "-s" << '\t'
            << "Same as --discover, but for an already running" << programName << "instance.";

    qInfo() << "--verbose"
            << '\t' << '|' << "-v" << '\t'
            << "Enable verbose log.";

    qInfo() << "--version"
            << '\t' << '|' << "-V" << '\t'
            << "Show this program version.";
    qInfo();
}
