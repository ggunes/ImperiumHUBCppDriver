#ifndef HIDDEVICE_H
#define HIDDEVICE_H

#include <QObject>
#include <QByteArray>

class HidDevice : public QObject {
    Q_OBJECT

public:
    explicit HidDevice(QObject *parent = nullptr);
    ~HidDevice();
    
    bool connect(uint16_t vendorId, uint16_t productId);
    void disconnect();
    bool isConnected() const;
    
    // Cihaza veri gönderme
    bool sendData(const QByteArray &data);
    
    // Cihazdan veri alma
    QByteArray receiveData(int timeout = 1000);
    
    // Cihaz bilgilerini alma
    QString getManufacturer() const;
    QString getProduct() const;
    QString getSerialNumber() const;
    
signals:
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &errorMessage);

private:
    void *deviceHandle; // HANDLE türü için void* kullanıyoruz (platform bağımsızlık için)
    bool connected;
    
    QString manufacturer;
    QString product;
    QString serialNumber;
    
    bool readDeviceInfo();
};

#endif // HIDDEVICE_H