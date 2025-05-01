#ifndef MOUSECONTROLLER_H
#define MOUSECONTROLLER_H

#include <QObject>
#include <QColor>
#include "../usb/devicemanager.h"

class MouseController : public QObject
{
    Q_OBJECT

public:
    explicit MouseController(DeviceManager *deviceManager, const QString &deviceId, QObject *parent = nullptr);
    
    // DPI ayarları
    bool setDpiLevel(int level, int dpi, const QColor &color);
    bool setDpiLevelEnabled(int level, bool enabled);
    
    // Polling rate
    bool setPollingRate(int rate);
    
    // RGB ayarları
    bool setRgbMode(int mode);
    bool setLogoColor(const QColor &color);
    bool setScrollColor(const QColor &color);
    bool setRgbBrightness(int brightness);
    bool setRgbSpeed(int speed);
    
    // Performans ayarları
    bool setLiftOffDistance(int distance);
    bool setAngleSnapping(bool enabled);
    bool setMotionSync(bool enabled);
    bool setDebounceTime(double time);
    bool calibrateSurface();
    
    // Tuş atamaları
    bool setButtonFunction(int buttonIndex, int function, int action, const QString &macroName);
    
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

#endif // MOUSECONTROLLER_H