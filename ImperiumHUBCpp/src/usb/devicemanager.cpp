#include "devicemanager.h"

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent),
      scanTimer(nullptr)
{
    // Periyodik tarama için zamanlayıcı oluştur
    scanTimer = new QTimer(this);
    connect(scanTimer, &QTimer::timeout, this, &DeviceManager::scanDevices);
    scanTimer->start(2000); // 2 saniyede bir tara
    
    // İlk taramayı başlat
    scanDevices();
}

DeviceManager::~DeviceManager()
{
    if (scanTimer) {
        scanTimer->stop();
    }
}

void DeviceManager::scanDevices()
{
    // Bu fonksiyon gerçek bir USB taraması yapacak
    // Şimdilik test için örnek cihazlar oluşturalım
    
    // Mevcut cihazları temizle
    connectedDevices.clear();
    
    // Test için örnek cihazlar ekle
    ConnectedDevice keyboard;
    keyboard.name = "ImperiumGear K1 Klavye";
    keyboard.serialNumber = "IG-K1-12345";
    keyboard.productId = 0x1234;
    keyboard.type = DeviceType::Keyboard;
    connectedDevices.append(keyboard);
    
    ConnectedDevice mouse;
    mouse.name = "ImperiumGear M1 Fare";
    mouse.serialNumber = "IG-M1-67890";
    mouse.productId = 0x5678;
    mouse.type = DeviceType::Mouse;
    connectedDevices.append(mouse);
    
    // Cihaz bağlantı sinyallerini gönder
    for (const ConnectedDevice& device : connectedDevices) {
        emit deviceConnected(device);
    }
}

QList<ConnectedDevice> DeviceManager::getConnectedDevices()
{
    return connectedDevices;
}