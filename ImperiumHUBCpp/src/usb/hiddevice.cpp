#include "hiddevice.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#include <setupapi.h>
#include <hidsdi.h>
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")
#endif

HidDevice::HidDevice(QObject *parent)
    : QObject(parent),
      deviceHandle(nullptr),
      connected(false)
{
}

HidDevice::~HidDevice() {
    disconnect();
}

bool HidDevice::connect(uint16_t vendorId, uint16_t productId) {
#ifdef Q_OS_WIN
    // Windows'ta HID cihazlarını bul
    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);
    
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&hidGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        emit errorOccurred(tr("HID cihazları bulunamadı"));
        return false;
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
            HANDLE handle = CreateFile(
                detailData->DevicePath,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                nullptr,
                OPEN_EXISTING,
                0,
                nullptr
            );
            
            if (handle != INVALID_HANDLE_VALUE) {
                // VID/PID bilgilerini al
                HIDD_ATTRIBUTES deviceAttributes;
                deviceAttributes.Size = sizeof(HIDD_ATTRIBUTES);
                
                if (HidD_GetAttributes(handle, &deviceAttributes)) {
                    // VID/PID eşleşiyor mu kontrol et
                    if (deviceAttributes.VendorID == vendorId && 
                        deviceAttributes.ProductID == productId) {
                        
                        deviceHandle = handle;
                        connected = true;
                        
                        // Cihaz bilgilerini oku
                        readDeviceInfo();
                        
                        free(detailData);
                        SetupDiDestroyDeviceInfoList(deviceInfoSet);
                        return true;
                    }
                }
                
                CloseHandle(handle);
            }
        }
        
        free(detailData);
    }
    
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    
    emit errorOccurred(tr("Belirtilen VID/PID ile cihaz bulunamadı"));
    return false;
#else
    // Diğer platformlar için uygun kod eklenebilir
    Q_UNUSED(vendorId);
    Q_UNUSED(productId);
    qDebug() << "HID cihaz bağlantısı sadece Windows'ta destekleniyor";
    return false;
#endif
}

void HidDevice::disconnect() {
#ifdef Q_OS_WIN
    if (deviceHandle) {
        CloseHandle(static_cast<HANDLE>(deviceHandle));
        deviceHandle = nullptr;
    }
#endif
    
    connected = false;
}

bool HidDevice::isConnected() const {
    return connected;
}

bool HidDevice::sendData(const QByteArray &data) {
#ifdef Q_OS_WIN
    if (!isConnected()) {
        emit errorOccurred(tr("Veri göndermek için cihaz bağlı değil"));
        return false;
    }
    
    // Rapor ID'si ile veri gönder
    DWORD bytesWritten = 0;
    BOOL result = WriteFile(
        static_cast<HANDLE>(deviceHandle),
        data.constData(),
        data.size(),
        &bytesWritten,
        nullptr
    );
    
    if (!result || bytesWritten != static_cast<DWORD>(data.size())) {
        emit errorOccurred(tr("Veri gönderme hatası"));
        return false;
    }
    
    return true;
#else
    Q_UNUSED(data);
    qDebug() << "HID veri gönderme sadece Windows'ta destekleniyor";
    return false;
#endif
}

QByteArray HidDevice::receiveData(int timeout) {
#ifdef Q_OS_WIN
    if (!isConnected()) {
        emit errorOccurred(tr("Veri almak için cihaz bağlı değil"));
        return QByteArray();
    }
    
    // Okuma için zaman aşımı ayarla
    COMMTIMEOUTS timeouts;
    GetCommTimeouts(static_cast<HANDLE>(deviceHandle), &timeouts);
    timeouts.ReadIntervalTimeout = timeout;
    timeouts.ReadTotalTimeoutConstant = timeout;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    SetCommTimeouts(static_cast<HANDLE>(deviceHandle), &timeouts);
    
    // Veriyi oku
    char buffer[256]; // HID raporları genellikle 64 veya 256 bayt
    DWORD bytesRead = 0;
    
    BOOL result = ReadFile(
        static_cast<HANDLE>(deviceHandle),
        buffer,
        sizeof(buffer),
        &bytesRead,
        nullptr
    );
    
    if (!result) {
        DWORD error = GetLastError();
        if (error != ERROR_TIMEOUT) {
            emit errorOccurred(tr("Veri okuma hatası: %1").arg(error));
        }
        return QByteArray();
    }
    
    QByteArray receivedData(buffer, bytesRead);
    emit dataReceived(receivedData);
    
    return receivedData;
#else
    Q_UNUSED(timeout);
    qDebug() << "HID veri alma sadece Windows'ta destekleniyor";
    return QByteArray();
#endif
}

QString HidDevice::getManufacturer() const {
    return manufacturer;
}

QString HidDevice::getProduct() const {
    return product;
}

QString HidDevice::getSerialNumber() const {
    return serialNumber;
}

bool HidDevice::readDeviceInfo() {
#ifdef Q_OS_WIN
    if (!isConnected()) {
        return false;
    }
    
    // Üretici bilgisini oku
    wchar_t wManufacturer[256] = {0};
    if (HidD_GetManufacturerString(static_cast<HANDLE>(deviceHandle), wManufacturer, sizeof(wManufacturer))) {
        manufacturer = QString::fromWCharArray(wManufacturer);
    } else {
        manufacturer = tr("Bilinmiyor");
    }
    
    // Ürün bilgisini oku
    wchar_t wProduct[256] = {0};
    if (HidD_GetProductString(static_cast<HANDLE>(deviceHandle), wProduct, sizeof(wProduct))) {
        product = QString::fromWCharArray(wProduct);
    } else {
        product = tr("Bilinmiyor");
    }
    
    // Seri numarasını oku
    wchar_t wSerialNumber[256] = {0};
    if (HidD_GetSerialNumberString(static_cast<HANDLE>(deviceHandle), wSerialNumber, sizeof(wSerialNumber))) {
        serialNumber = QString::fromWCharArray(wSerialNumber);
    } else {
        serialNumber = tr("Bilinmiyor");
    }
    
    return true;
#else
    qDebug() << "HID cihaz bilgisi okuma sadece Windows'ta destekleniyor";
    manufacturer = tr("Bilinmiyor");
    product = tr("Bilinmiyor");
    serialNumber = tr("Bilinmiyor");
    return false;
#endif
}