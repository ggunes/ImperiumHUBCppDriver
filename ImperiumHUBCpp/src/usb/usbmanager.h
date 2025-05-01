#ifndef USBMANAGER_H
#define USBMANAGER_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include "hiddevice.h"

class UsbManager : public QObject {
    Q_OBJECT

public:
    explicit UsbManager(QObject *parent = nullptr);
    ~UsbManager();
    
    void startDeviceDiscovery();
    void stopDeviceDiscovery();
    
    bool isDeviceConnected() const;
    HidDevice* getCurrentDevice() const;

signals:
    void deviceConnected(const QString &deviceName, const QString &model, const QString &firmware);
    void deviceDisconnected();
    void errorOccurred(const QString &errorMessage);

private slots:
    void scanDevices();

private:
    QTimer *scanTimer;
    HidDevice *currentDevice;
    QMap<QString, QPair<uint16_t, uint16_t>> supportedDevices; // name -> (VID, PID)
    
    void setupSupportedDevices();
    bool connectToDevice(uint16_t vendorId, uint16_t productId);
    void disconnectCurrentDevice();
};

#endif // USBMANAGER_H