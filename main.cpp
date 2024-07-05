#include <QApplication>
#include <QDebug>
#include <QLocale>
#include <QTranslator>

#include <getopt.h>

#include "bluetoothlistener.hpp"
#include "screenlocker.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "BtScreenLocker_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    const struct option longOptions[] = {
        { "debug",      no_argument,    0,  'D' },
        { "discover",   no_argument,    0,  'd' },
        { "verbose",    no_argument,    0,  'v' },
        { "version",    no_argument,    0,  'V' },
        { nullptr, 0, nullptr, 0 }
    };

    bool debug {false};
    bool discover {false};
    bool verbose {false};
    int option = 0;
    while ((option = getopt_long(argc, argv, "DdvV", longOptions, nullptr)) >= 0) {
        switch (option)
        {
        case 'D':
            debug = true;
            break;
        case 'd':
            discover = true;
            break;
        case 'v':
            verbose = true;
            break;
        case 'V':
            qInfo() << PROJECT_NAME << "version" << PROJECT_VERSION;
            return 0;
        default:
            qCritical() << a.tr("Unknown option:") << optopt;
        }
    }

    ScreenLocker locker;
    BluetoothListener listener(locker);
    a.connect(&listener, &BluetoothListener::lockScreen, &locker, &ScreenLocker::lockScreen);
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
    }

    return a.exec();
}
