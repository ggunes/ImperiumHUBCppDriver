#ifndef KEYBOARDCONTROLLER_H
#define KEYBOARDCONTROLLER_H

#include <QObject>
#include <QColor>
#include "../usb/devicemanager.h"

struct KeyConfig {
    int function;
    int keyCode;
    QString macroName;
};

class KeyboardController : public QObject
{
    Q_OBJECT

public:
    explicit KeyboardController(DeviceManager *deviceManager, const QString &deviceId, QObject *parent = nullptr);
    
    // RGB ayarları
    bool setRgbMode(int mode);
    bool setRgbColor(const QColor &color);
    bool setRgbBrightness(int brightness);
    bool setRgbSpeed(int speed);
    bool setRgbDirection(int direction);
    
    // Performans ayarları
    bool setPollingRate(int rate);
    bool setDebounceTime(int time);
    bool setNkroMode(int mode);
    bool setGameMode(bool enabled);
    bool setWinKeyLock(bool enabled);
    
    // Hall Effect ayarları
    bool setActuationPoint(double point);
    bool setRapidTrigger(bool enabled);
    bool setDynamicKeystroke(bool enabled);
    
    // Tuş atamaları
    bool setKeyFunction(int keyIndex, const KeyConfig &config);
    
    // Varsayılan ayarlar
    bool resetToDefaults();
    
    // Cihaz bilgileri
    QString getDeviceId() const;
    bool isConnected() const;

private:
    DeviceManager *deviceManager;
    QString deviceId;
    
    // Komut oluşturma
    QByteArray createCommand(const QByteArray &commandId, const QByteArray &data);
    
    // Komut gönderme
    bool sendCommand(const QByteArray &command);
    
    // Yanıt okuma
    QByteArray readResponse();
};

#endif // KEYBOARDCONTROLLER_H