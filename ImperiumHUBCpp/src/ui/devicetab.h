#ifndef DEVICETAB_H
#define DEVICETAB_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "../usb/devicemanager.h"

class DeviceTab : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceTab(DeviceManager *deviceManager, QWidget *parent = nullptr);
    
    // Cihaz bağlantı durumu değiştiğinde
    void onDeviceConnected(const ConnectedDevice& device);
    void onDeviceDisconnected(const ConnectedDevice& device);

private slots:
    // Cihaz listesi seçimi değiştiğinde
    void onDeviceSelectionChanged();
    
    // Cihaz taramasını yenile
    void refreshDevices();

private:
    // Arayüz kurulumu
    void setupUi();
    
    // Cihaz bilgilerini güncelle
    void updateDeviceInfo(const ConnectedDevice& device);
    void clearDeviceInfo();
    
    DeviceManager *deviceManager;
    
    // Arayüz bileşenleri
    QListWidget *deviceListWidget;
    QLabel *deviceImageLabel;
    QLabel *deviceNameLabel;
    QLabel *deviceTypeLabel;
    QLabel *deviceSerialLabel;
    QLabel *deviceFirmwareLabel;
    QPushButton *refreshButton;
    
    QGroupBox *deviceInfoGroup;
};

#endif // DEVICETAB_H