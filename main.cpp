#include <QApplication>
#include <QDebug>
#include <QLocale>
#include <QTranslator>

#include "bluetoothlistener.hpp"
#include "screenlocker.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BluetoothListener listener;

    if (argc > 1) {
        auto arg = QString(argv[1]);
        if (arg == "--version") {
            qInfo() << PROJECT_NAME << "version" << PROJECT_VERSION;
            return 0;
        } else if (arg == "--discover") {
            listener.startDiscovery();
        }
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "BtScreenLocker_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    ScreenLocker locker;
    a.connect(&listener, &BluetoothListener::lock, &locker, &ScreenLocker::lockScreen);

    return a.exec();
}
