#include "devicechooser.hpp"
#include "ui_devicechooser.h"

DeviceChooser::DeviceChooser(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::DeviceChooser)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Discovered Devices"));

    configTable();

    connect(m_ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &DeviceChooser::onCellDoubleClicked);
    connect(m_ui->addButton, &QPushButton::clicked, this, &DeviceChooser::onAddButtonClicked);
}

DeviceChooser::~DeviceChooser()
{
    delete m_ui;
}

void DeviceChooser::closeEvent(QCloseEvent *event)
{
    emit closed();
    QWidget::closeEvent(event);
}

void DeviceChooser::showEvent(QShowEvent *event)
{
    m_ui->tableWidget->resizeColumnsToContents();
    QWidget::showEvent(event);
}

void DeviceChooser::configTable()
{
    QStringList headers;
    headers << tr("Name") << tr("MAC Address") << tr("Signal strength");

    m_ui->tableWidget->setColumnCount(headers.size());
    m_ui->tableWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    m_ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_ui->tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
    m_ui->tableWidget->setHorizontalHeaderLabels(headers);
}

void DeviceChooser::onCellDoubleClicked(int row, int column)
{
    auto name = m_ui->tableWidget->item(row, 0)->text();
    auto address = m_ui->tableWidget->item(row, 1)->text();
    m_selectedDevices[name] = address;
    close();
}

void DeviceChooser::onAddButtonClicked()
{
    for (const auto *item : m_ui->tableWidget->selectedItems()) {
        int row = m_ui->tableWidget->row(item);
        auto name = m_ui->tableWidget->item(row, 0)->text();
        auto address = m_ui->tableWidget->item(row, 1)->text();
        m_selectedDevices[name] = address;
    }

    close();
}

void DeviceChooser::addDevice(const QString &name,
                              const QString &address,
                              const QString &rssi)
{
    int rowCount = m_ui->tableWidget->rowCount();
    m_ui->tableWidget->insertRow(rowCount);
    m_ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(name));
    m_ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(address));
    m_ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(rssi));
}

std::map<QString, QString> DeviceChooser::selectedDevices()
{
    return m_selectedDevices;
}
