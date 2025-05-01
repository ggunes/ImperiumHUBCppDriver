#include "devicetab.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QPixmap>
#include "../usb/devicemanager.h" // DeviceManager sınıfı için

DeviceTab::DeviceTab(DeviceManager *deviceManager, QWidget *parent)
    : QWidget(parent),
      deviceManager(deviceManager)
{
    setupUi();
    
    // Bağlı cihazları listele
    QList<ConnectedDevice> devices = deviceManager->getConnectedDevices();
    for (const ConnectedDevice& device : devices) {
        QListWidgetItem *item = new QListWidgetItem(device.name);
        item->setData(Qt::UserRole, device.productId);
        deviceListWidget->addItem(item);
    }
    
    // Bağlantıları kur
    connect(deviceListWidget, &QListWidget::currentItemChanged, 
            this, &DeviceTab::onDeviceSelectionChanged);
    connect(refreshButton, &QPushButton::clicked,
            this, &DeviceTab::refreshDevices);
}

void DeviceTab::setupUi()
{
    // Ana düzen
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    
    // Sol panel - Cihaz listesi
    QVBoxLayout *leftLayout = new QVBoxLayout();
    
    QLabel *devicesLabel = new QLabel(tr("Bağlı Cihazlar"), this);
    devicesLabel->setStyleSheet("font-weight: bold;");
    leftLayout->addWidget(devicesLabel);
    
    deviceListWidget = new QListWidget(this);
    deviceListWidget->setMinimumWidth(200);
    leftLayout->addWidget(deviceListWidget);
    
    refreshButton = new QPushButton(tr("Yenile"), this);
    leftLayout->addWidget(refreshButton);
    
    mainLayout->addLayout(leftLayout);
    
    // Sağ panel - Cihaz bilgileri
    QVBoxLayout *rightLayout = new QVBoxLayout();
    
    deviceInfoGroup = new QGroupBox(tr("Cihaz Bilgileri"), this);
    QVBoxLayout *infoLayout = new QVBoxLayout(deviceInfoGroup);
    
    // Cihaz görseli
    deviceImageLabel = new QLabel(this);
    deviceImageLabel->setAlignment(Qt::AlignCenter);
    deviceImageLabel->setMinimumHeight(200);
    infoLayout->addWidget(deviceImageLabel);
    
    // Cihaz bilgileri
    QGridLayout *detailsLayout = new QGridLayout();
    
    detailsLayout->addWidget(new QLabel(tr("Cihaz Adı:"), this), 0, 0);
    deviceNameLabel = new QLabel(this);
    detailsLayout->addWidget(deviceNameLabel, 0, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Cihaz Türü:"), this), 1, 0);
    deviceTypeLabel = new QLabel(this);
    detailsLayout->addWidget(deviceTypeLabel, 1, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Seri Numarası:"), this), 2, 0);
    deviceSerialLabel = new QLabel(this);
    detailsLayout->addWidget(deviceSerialLabel, 2, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Firmware Sürümü:"), this), 3, 0);
    deviceFirmwareLabel = new QLabel(this);
    detailsLayout->addWidget(deviceFirmwareLabel, 3, 1);
    
    infoLayout->addLayout(detailsLayout);
    infoLayout->addStretch();
    
    rightLayout->addWidget(deviceInfoGroup);
    mainLayout->addLayout(rightLayout, 1);
    
    // Başlangıçta cihaz bilgilerini gizle
    deviceInfoGroup->setVisible(false);
}

void DeviceTab::onDeviceConnected(const ConnectedDevice& device)
{
    // Cihaz listesine ekle
    QListWidgetItem *item = new QListWidgetItem(device.name);
    item->setData(Qt::UserRole, device.productId);
    deviceListWidget->addItem(item);
}

void DeviceTab::onDeviceDisconnected(const ConnectedDevice& device)
{
    // Cihazı listeden kaldır
    for (int i = 0; i < deviceListWidget->count(); i++) {
        QListWidgetItem *item = deviceListWidget->item(i);
        if (item->data(Qt::UserRole).toUInt() == device.productId) {
            delete deviceListWidget->takeItem(i);
            break;
        }
    }
    
    // Eğer seçili cihaz kaldırıldıysa bilgileri temizle
    if (deviceListWidget->count() == 0) {
        clearDeviceInfo();
    }
}

void DeviceTab::onDeviceSelectionChanged()
{
    QListWidgetItem *currentItem = deviceListWidget->currentItem();
    if (!currentItem) {
        clearDeviceInfo();
        return;
    }
    
    uint16_t productId = currentItem->data(Qt::UserRole).toUInt();
    
    // Seçilen cihazı bul
    QList<ConnectedDevice> devices = deviceManager->getConnectedDevices();
    for (const ConnectedDevice& device : devices) {
        if (device.productId == productId) {
            updateDeviceInfo(device);
            return;
        }
    }
    
    // Cihaz bulunamadıysa bilgileri temizle
    clearDeviceInfo();
}

void DeviceTab::refreshDevices()
{
    // Cihaz listesini temizle
    deviceListWidget->clear();
    
    // Cihaz taramasını yenile
    deviceManager->scanDevices();
    
    // Bağlı cihazları listele
    QList<ConnectedDevice> devices = deviceManager->getConnectedDevices();
    for (const ConnectedDevice& device : devices) {
        QListWidgetItem *item = new QListWidgetItem(device.name);
        item->setData(Qt::UserRole, device.productId);
        deviceListWidget->addItem(item);
    }
    
    // Bilgileri temizle
    clearDeviceInfo();
}

void DeviceTab::updateDeviceInfo(const ConnectedDevice& device)
{
    deviceInfoGroup->setVisible(true);
    
    // Cihaz görseli
    QPixmap deviceImage;
    if (device.type == DeviceType::Keyboard) {
        deviceImage.load(":/images/keyboard.png");
    } else if (device.type == DeviceType::Mouse) {
        deviceImage.load(":/images/mouse.png");
    } else {
        deviceImage.load(":/images/unknown.png");
    }
    
    deviceImageLabel->setPixmap(deviceImage.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    
    // Cihaz bilgileri
    deviceNameLabel->setText(device.name);
    
    QString deviceType;
    if (device.type == DeviceType::Keyboard) {
        deviceType = tr("Klavye");
    } else if (device.type == DeviceType::Mouse) {
        deviceType = tr("Fare");
    } else {
        deviceType = tr("Bilinmiyor");
    }
    deviceTypeLabel->setText(deviceType);
    
    deviceSerialLabel->setText(device.serialNumber);
    
    // Firmware bilgisi
    if (device.type == DeviceType::Keyboard) {
        // Klavye kontrolcüsünden firmware bilgisini al
        // Bu kısım ileride uygulanacak
        deviceFirmwareLabel->setText(tr("Yükleniyor..."));
    } else if (device.type == DeviceType::Mouse) {
        // Fare kontrolcüsünden firmware bilgisini al
        // Bu kısım ileride uygulanacak
        deviceFirmwareLabel->setText(tr("Yükleniyor..."));
    } else {
        deviceFirmwareLabel->setText(tr("Bilinmiyor"));
    }
}

void DeviceTab::clearDeviceInfo()
{
    deviceInfoGroup->setVisible(false);
    deviceImageLabel->clear();
    deviceNameLabel->clear();
    deviceTypeLabel->clear();
    deviceSerialLabel->clear();
    deviceFirmwareLabel->clear();
}