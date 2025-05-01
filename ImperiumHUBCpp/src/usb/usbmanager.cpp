#include "usbmanager.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#include <setupapi.h>
#include <hidsdi.h>
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")
#endif

UsbManager::UsbManager(QObject *parent)
    : QObject(parent),
      scanTimer(new QTimer(this)),
      currentDevice(nullptr)
{
    // Desteklenen cihazları ayarla
    setupSupportedDevices();
    
    // Tarama zamanlayıcısını ayarla (her 2 saniyede bir)
    scanTimer->setInterval(2000);
    connect(scanTimer, &QTimer::timeout, this, &UsbManager::scanDevices);
}

UsbManager::~UsbManager() {
    stopDeviceDiscovery();
    disconnectCurrentDevice();
}

void UsbManager::setupSupportedDevices() {
    // Imperium I75 Klavye
    supportedDevices["Imperium I75"] = qMakePair(0x1234, 0x5678); // Örnek VID/PID, gerçek değerlerle değiştirilmeli
    
    // Imperium Sentinel X1 Mouse
    supportedDevices["Imperium Sentinel X1"] = qMakePair(0x1234, 0x5679); // Örnek VID/PID, gerçek değerlerle değiştirilmeli
}

void UsbManager::startDeviceDiscovery() {
    scanDevices(); // İlk taramayı hemen yap
    scanTimer->start(); // Periyodik taramayı başlat
}

void UsbManager::stopDeviceDiscovery() {
    scanTimer->stop();
}

bool UsbManager::isDeviceConnected() const {
    return currentDevice != nullptr;
}

HidDevice* UsbManager::getCurrentDevice() const {
    return currentDevice;
}

void UsbManager::scanDevices() {
#ifdef Q_OS_WIN
    // Windows'ta HID cihazlarını tara
    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);
    
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&hidGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        emit errorOccurred(tr("HID cihazları taranamadı"));
        return;
    }
    
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    
    // Tüm HID cihazlarını kontrol et
    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, nullptr, &hidGuid, i, &deviceInterfaceData); i++) {
        DWORD requiredSize = 0;
        
        // Gerekli boyutu al
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, nullptr, 0, &requiredSize, nullptr);
        
        if (requiredSize == 0) {
            continue;
        }
        
        // Bellek ayır
        PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = 
            (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
        
        if (!detailData) {
            continue;
        }
        
        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        
        // Detayları al
        if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, detailData, requiredSize, nullptr, nullptr)) {
            // Cihazı aç
            HANDLE deviceHandle = CreateFile(
                detailData->DevicePath,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                nullptr,
                OPEN_EXISTING,
                0,
                nullptr
            );
            
            if (deviceHandle != INVALID_HANDLE_VALUE) {
                // VID/PID bilgilerini al
                HIDD_ATTRIBUTES deviceAttributes;
                deviceAttributes.Size = sizeof(HIDD_ATTRIBUTES);
                
                if (HidD_GetAttributes(deviceHandle, &deviceAttributes)) {
                    // Desteklenen cihazları kontrol et
                    for (auto it = supportedDevices.begin(); it != supportedDevices.end(); ++it) {
                        if (deviceAttributes.VendorID == it.value().first && 
                            deviceAttributes.ProductID == it.value().second) {
                            
                            // Eğer zaten bağlı değilse, bağlan
                            if (!isDeviceConnected()) {
                                if (connectToDevice(deviceAttributes.VendorID, deviceAttributes.ProductID)) {
                                    qDebug() << "Cihaz bağlandı:" << it.key();
                                    emit deviceConnected(it.key(), "Model " + QString::number(deviceAttributes.ProductID), "v1.0");
                                }
                            }
                            
                            CloseHandle(deviceHandle);
                            free(detailData);
                            SetupDiDestroyDeviceInfoList(deviceInfoSet);
                            return;
                        }
                    }
                }
                
                CloseHandle(deviceHandle);
            }
        }
        
        free(detailData);
    }
    
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    
    // Eğer buraya kadar geldiyse ve bir cihaz bağlıysa, bağlantıyı kes
    if (isDeviceConnected()) {
        disconnectCurrentDevice();
        emit deviceDisconnected();
    }
#else
    // Diğer platformlar için uygun kod eklenebilir
    qDebug() << "USB tarama sadece Windows'ta destekleniyor";
#endif
}

bool UsbManager::connectToDevice(uint16_t vendorId, uint16_t productId) {
    // Mevcut cihazı kapat
    disconnectCurrentDevice();
    
    // Yeni cihaz oluştur
    currentDevice = new HidDevice(this);
    
    // Cihaza bağlan
    if (!currentDevice->connect(vendorId, productId)) {
        delete currentDevice;
        currentDevice = nullptr;
        return false;
    }
    
    return true;
}

void UsbManager::disconnectCurrentDevice() {
    if (currentDevice) {
        currentDevice->disconnect();
        delete currentDevice;
        currentDevice = nullptr;
    }
}