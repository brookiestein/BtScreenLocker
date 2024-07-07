#ifndef DEVICECHOOSER_HPP
#define DEVICECHOOSER_HPP

#include <map>
#include <QCloseEvent>
#include <QShowEvent>
#include <QWidget>

namespace Ui {
class DeviceChooser;
}

class DeviceChooser : public QWidget
{
    Q_OBJECT
    Ui::DeviceChooser *m_ui;
    std::map<QString, QString> m_selectedDevice;

    void configTable();
public:
    explicit DeviceChooser(QWidget *parent = nullptr);
    ~DeviceChooser();
    void addDevice(const QString &name, const QString &address, const QString rssi);
    std::map<QString, QString> selectedDevice();
protected:
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);
signals:
    void closed();
private slots:
    void deviceClicked(int row, int column);
};

#endif // DEVICECHOOSER_HPP
