#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QTimer>

// Cihaz türleri
enum class DeviceType {
    Unknown,
    Keyboard,
    Mouse
};

// Bağlı cihaz bilgileri
struct ConnectedDevice {
    QString name;
    QString serialNumber;
    uint16_t productId;
    DeviceType type;
};

class DeviceManager : public QObject
{
    Q_OBJECT

public:
    explicit DeviceManager(QObject *parent = nullptr);
    ~DeviceManager();

    // Cihaz tarama
    void scanDevices();
    QList<ConnectedDevice> getConnectedDevices();

signals:
    void deviceConnected(const ConnectedDevice& device);
    void deviceDisconnected(const ConnectedDevice& device);

private:
    QList<ConnectedDevice> connectedDevices;
    QTimer *scanTimer;
};

#endif // DEVICEMANAGER_H