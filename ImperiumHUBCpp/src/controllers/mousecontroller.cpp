#include "mousecontroller.h"
#include <QDebug>

MouseController::MouseController(HidDevice* device, QObject *parent)
    : QObject(parent),
      device(device),
      activeDpiLevel(0),
      currentPollingRate(PollingRate::Hz1000),
      logoColor(Qt::white),
      scrollColor(Qt::white),
      rgbMode(1), // Statik
      rgbBrightness(100),
      rgbSpeed(50),
      liftOffDistance(2),
      angleSnappingEnabled(false),
      motionSyncEnabled(true),
      debounceTime(4.0)
{
    // Cihaz bağlantısını kontrol et
    if (!device || !device->isConnected()) {
        qDebug() << "Fare kontrolcüsü oluşturulurken cihaz bağlantısı bulunamadı";
        return;
    }
    
    // Varsayılan DPI seviyeleri
    for (int i = 0; i < 5; i++) {
        DpiLevel level;
        level.dpi = 800 * (i + 1); // 800, 1600, 2400, 3200, 4000
        level.color = QColor::fromHsv(i * 60, 255, 255); // Farklı renkler
        level.enabled = (i < 3); // İlk 3 seviye aktif
        dpiLevels[i] = level;
    }
    
    // Firmware ve sensör bilgilerini al
    QByteArray command;
    command.append(static_cast<char>(CMD_GET_FIRMWARE));
    sendCommand(command);
    
    command.clear();
    command.append(static_cast<char>(CMD_GET_SENSOR));
    sendCommand(command);
}

MouseController::~MouseController() {
    // Temizleme işlemleri gerekirse burada yapılabilir
}

bool MouseController::setDpiLevel(int level, int dpi, const QColor& color) {
    if (level < 0 || level > 4) {
        emit errorOccurred(tr("Geçersiz DPI seviyesi: %1").arg(level));
        return false;
    }
    
    if (dpi < 50 || dpi > 26000) {
        emit errorOccurred(tr("Geçersiz DPI değeri: %1").arg(dpi));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_DPI_LEVEL));
    command.append(static_cast<char>(level));
    
    // DPI değerini 2 byte olarak gönder (little endian)
    command.append(static_cast<char>(dpi & 0xFF));
    command.append(static_cast<char>((dpi >> 8) & 0xFF));
    
    // Renk değerlerini gönder
    command.append(static_cast<char>(color.red()));
    command.append(static_cast<char>(color.green()));
    command.append(static_cast<char>(color.blue()));
    
    // Etkin/devre dışı durumunu gönder
    command.append(static_cast<char>(dpiLevels[level].enabled ? 1 : 0));
    
    if (sendCommand(command)) {
        dpiLevels[level].dpi = dpi;
        dpiLevels[level].color = color;
        emit commandSuccess(tr("DPI seviyesi başarıyla güncellendi"));
        return true;
    }
    
    return false;
}

bool MouseController::enableDpiLevel(int level, bool enabled) {
    if (level < 0 || level > 4) {
        emit errorOccurred(tr("Geçersiz DPI seviyesi: %1").arg(level));
        return false;
    }
    
    if (!dpiLevels.contains(level)) {
        emit errorOccurred(tr("DPI seviyesi bulunamadı: %1").arg(level));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_DPI_LEVEL));
    command.append(static_cast<char>(level));
    
    // Mevcut DPI değerini 2 byte olarak gönder
    int dpi = dpiLevels[level].dpi;
    command.append(static_cast<char>(dpi & 0xFF));
    command.append(static_cast<char>((dpi >> 8) & 0xFF));
    
    // Mevcut renk değerlerini gönder
    QColor color = dpiLevels[level].color;
    command.append(static_cast<char>(color.red()));
    command.append(static_cast<char>(color.green()));
    command.append(static_cast<char>(color.blue()));
    
    // Yeni etkin/devre dışı durumunu gönder
    command.append(static_cast<char>(enabled ? 1 : 0));
    
    if (sendCommand(command)) {
        dpiLevels[level].enabled = enabled;
        emit commandSuccess(tr("DPI seviyesi %1").arg(enabled ? "etkinleştirildi" : "devre dışı bırakıldı"));
        return true;
    }
    
    return false;
}

DpiLevel MouseController::getDpiLevel(int level) const {
    if (dpiLevels.contains(level)) {
        return dpiLevels[level];
    }
    
    // Varsayılan değer
    DpiLevel defaultLevel;
    defaultLevel.dpi = 800;
    defaultLevel.color = Qt::white;
    defaultLevel.enabled = false;
    
    return defaultLevel;
}

bool MouseController::setActiveDpiLevel(int level) {
    if (level < 0 || level > 4) {
        emit errorOccurred(tr("Geçersiz DPI seviyesi: %1").arg(level));
        return false;
    }
    
    if (!dpiLevels.contains(level) || !dpiLevels[level].enabled) {
        emit errorOccurred(tr("DPI seviyesi etkin değil: %1").arg(level));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_DPI_ACTIVE));
    command.append(static_cast<char>(level));
    
    if (sendCommand(command)) {
        activeDpiLevel = level;
        emit commandSuccess(tr("Aktif DPI seviyesi değiştirildi: %1").arg(dpiLevels[level].dpi));
        return true;
    }
    
    return false;
}

bool MouseController::setPollingRate(PollingRate rate) {
    QByteArray command;
    command.append(static_cast<char>(CMD_POLLING_RATE));
    
    // Polling rate değerini 2 byte olarak gönder
    int rateValue = static_cast<int>(rate);
    command.append(static_cast<char>(rateValue & 0xFF));
    command.append(static_cast<char>((rateValue >> 8) & 0xFF));
    
    if (sendCommand(command)) {
        currentPollingRate = rate;
        emit commandSuccess(tr("Polling rate değiştirildi: %1Hz").arg(rateValue));
        return true;
    }
    
    return false;
}

PollingRate MouseController::getPollingRate() const {
    return currentPollingRate;
}

bool MouseController::setButtonFunction(int buttonIndex, const MouseButtonConfig& config) {
    if (buttonIndex < 0 || buttonIndex > 8) { // Maksimum 9 fare tuşu
        emit errorOccurred(tr("Geçersiz fare tuşu indeksi: %1").arg(buttonIndex));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_BUTTON_FUNCTION));
    command.append(static_cast<char>(buttonIndex));
    command.append(static_cast<char>(config.function));
    command.append(static_cast<char>(config.keyCode));
    
    // Makro adı için maksimum 16 karakter
    QByteArray macroNameBytes = config.macroName.toUtf8();
    macroNameBytes.truncate(16);
    command.append(macroNameBytes);
    
    if (sendCommand(command)) {
        buttonConfigurations[buttonIndex] = config;
        emit commandSuccess(tr("Fare tuşu fonksiyonu başarıyla değiştirildi"));
        return true;
    }
    
    return false;
}

MouseButtonConfig MouseController::getButtonFunction(int buttonIndex) const {
    if (buttonConfigurations.contains(buttonIndex)) {
        return buttonConfigurations[buttonIndex];
    }
    
    // Varsayılan değer
    MouseButtonConfig defaultConfig;
    defaultConfig.function = MouseButtonFunction::Default;
    defaultConfig.keyCode = 0;
    defaultConfig.macroName = "";
    
    return defaultConfig;
}

bool MouseController::setLogoColor(const QColor& color) {
    QByteArray command;
    command.append(static_cast<char>(CMD_RGB_LOGO));
    command.append(static_cast<char>(color.red()));
    command.append(static_cast<char>(color.green()));
    command.append(static_cast<char>(color.blue()));
    
    if (sendCommand(command)) {
        logoColor = color;
        emit commandSuccess(tr("Logo rengi başarıyla değiştirildi"));
        return true;
    }
    
    return false;
}

bool MouseController::setScrollColor(const QColor& color) {
    QByteArray command;
    command.append(static_cast<char>(CMD_RGB_SCROLL));
    command.append(static_cast<char>(color.red()));
    command.append(static_cast<char>(color.green()));
    command.append(static_cast<char>(color.blue()));
    
    if (sendCommand(command)) {
        scrollColor = color;
        emit commandSuccess(tr("Scroll rengi başarıyla değiştirildi"));
        return true;
    }
    
    return false;
}

bool MouseController::setRgbMode(int mode) {
    if (mode < 0 || mode > 3) {
        emit errorOccurred(tr("Geçersiz RGB modu: %1").arg(mode));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_RGB_MODE));
    command.append(static_cast<char>(mode));
    
    if (sendCommand(command)) {
        rgbMode = mode;
        emit commandSuccess(tr("RGB modu başarıyla değiştirildi"));
        return true;
    }
    
    return false;
}

bool MouseController::setRgbBrightness(int brightness) {
    if (brightness < 0 || brightness > 100) {
        emit errorOccurred(tr("Geçersiz parlaklık değeri: %1").arg(brightness));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_RGB_BRIGHTNESS));
    command.append(static_cast<char>(brightness));
    
    if (sendCommand(command)) {
        rgbBrightness = brightness;
        emit commandSuccess(tr("RGB parlaklığı başarıyla değiştirildi"));
        return true;
    }
    
    return false;
}

bool MouseController::setRgbSpeed(int speed) {
    if (speed < 0 || speed > 100) {
        emit errorOccurred(tr("Geçersiz hız değeri: %1").arg(speed));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_RGB_SPEED));
    command.append(static_cast<char>(speed));
    
    if (sendCommand(command)) {
        rgbSpeed = speed;
        emit commandSuccess(tr("RGB hızı başarıyla değiştirildi"));
        return true;
    }
    
    return false;
}

bool MouseController::setLiftOffDistance(int distance) {
    if (distance < 1 || distance > 5) {
        emit errorOccurred(tr("Geçersiz LOD değeri: %1").arg(distance));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_LOD));
    command.append(static_cast<char>(distance));
    
    if (sendCommand(command)) {
        liftOffDistance = distance;
        emit commandSuccess(tr("Lift-Off Distance başarıyla değiştirildi"));
        return true;
    }
    
    return false;
}

bool MouseController::setAngleSnapping(bool enabled) {
    QByteArray command;
    command.append(static_cast<char>(CMD_ANGLE_SNAPPING));
    command.append(static_cast<char>(enabled ? 1 : 0));
    
    if (sendCommand(command)) {
        angleSnappingEnabled = enabled;
        emit commandSuccess(tr("Angle Snapping %1").arg(enabled ? "etkinleştirildi" : "devre dışı bırakıldı"));
        return true;
    }
    
    return false;
}

bool MouseController::setMotionSync(bool enabled) {
    QByteArray command;
    command.append(static_cast<char>(CMD_MOTION_SYNC));
    command.append(static_cast<char>(enabled ? 1 : 0));
    
    if (sendCommand(command)) {
        motionSyncEnabled = enabled;
        emit commandSuccess(tr("Motion Sync %1").arg(enabled ? "etkinleştirildi" : "devre dışı bırakıldı"));
        return true;
    }
    
    return false;
}

bool MouseController::setDebounceTime(double time) {
    if (time < 0.2 || time > 16.0) {
        emit errorOccurred(tr("Geçersiz debounce süresi: %1ms").arg(time));
        return false;
    }
    
    // Debounce süresini 0.1ms hassasiyetle gönder (10 katını al)
    int timeValue = static_cast<int>(time * 10);
    
    QByteArray command;
    command.append(static_cast<char>(CMD_DEBOUNCE));
    command.append(static_cast<char>(timeValue & 0xFF));
    
    if (sendCommand(command)) {
        debounceTime = time;
        emit commandSuccess(tr("Debounce süresi başarıyla değiştirildi: %1ms").arg(time));
        return true;
    }
    
    return false;
}

bool MouseController::calibrateSurface() {
    QByteArray command;
    command.append(static_cast<char>(CMD_CALIBRATE));
    
    if (sendCommand(command)) {
        emit commandSuccess(tr("Yüzey kalibrasyonu başlatıldı"));
        return true;
    }
    
    return false;
}

bool MouseController::resetToDefaults() {
    QByteArray command;
    command.append(static_cast<char>(CMD_RESET));
    
    if (sendCommand(command)) {
        // Varsayılan değerleri yükle
        loadDefaultSettings();
        emit commandSuccess(tr("Fare ayarları varsayılan değerlere sıfırlandı"));
        return true;
    }
    
    return false;
}

void MouseController::loadDefaultSettings() {
    // Varsayılan DPI seviyeleri
    for (int i = 0; i < 5; i++) {
        DpiLevel level;
        level.dpi = 800 * (i + 1); // 800, 1600, 2400, 3200, 4000
        level.color = QColor::fromHsv(i * 60, 255, 255); // Farklı renkler
        level.enabled = (i < 3); // İlk 3 seviye aktif
        dpiLevels[i] = level;
    }
    
    activeDpiLevel = 0;
    currentPollingRate = PollingRate::Hz1000;
    logoColor = Qt::white;
    scrollColor = Qt::white;
    rgbMode = 1; // Statik
    rgbBrightness = 100;
    rgbSpeed = 50;
    liftOffDistance = 2;
    angleSnappingEnabled = false;
    motionSyncEnabled = true;
    debounceTime = 4.0;
    
    // Varsayılan tuş atamaları
    buttonConfigurations.clear();
}

bool MouseController::sendCommand(const QByteArray &data) {
    if (!device || !device->isConnected()) {
        emit errorOccurred(tr("Komut göndermek için cihaz bağlı değil"));
        return false;
    }
    
    // Komut başlığı ekle
    QByteArray command;
    command.append(static_cast<char>(0xA5)); // Başlangıç baytı
    command.append(static_cast<char>(data.size())); // Veri uzunluğu
    command.append(data); // Komut verisi
    
    // Checksum hesapla
    unsigned char checksum = 0;
    for (int i = 0; i < command.size(); i++) {
        checksum ^= static_cast<unsigned char>(command.at(i));
    }
    command.append(static_cast<char>(checksum)); // Checksum ekle
    
    // Komutu gönder
    if (!device->sendData(command)) {
        emit errorOccurred(tr("Komut gönderme hatası"));
        return false;
    }
    
    // Yanıt bekle
    QByteArray response = device->receiveData(500); // 500ms timeout
    
    if (response.isEmpty()) {
        emit errorOccurred(tr("Cihazdan yanıt alınamadı"));
        return false;
    }
    
    // Yanıtı doğrula
    if (response.size() < 2) {
        emit errorOccurred(tr("Geçersiz yanıt formatı"));
        return false;
    }
    
    // Başarı durumunu kontrol et
    if (static_cast<unsigned char>(response.at(0)) != 0xA5 || 
        static_cast<unsigned char>(response.at(1)) != 0x01) {
        emit errorOccurred(tr("Komut başarısız oldu"));
        return false;
    }
    
    return true;
}

void MouseController::handleResponse(const QByteArray &response) {
    if (response.size() < 3) {
        return;
    }
    
    // Yanıt başlığını kontrol et
    if (static_cast<unsigned char>(response.at(0)) != 0xA5) {
        return;
    }
    
    // Komut tipini al
    unsigned char cmdType = static_cast<unsigned char>(response.at(2));
    
    switch (cmdType) {
        case CMD_GET_FIRMWARE:
            if (response.size() >= 6) {
                int major = static_cast<unsigned char>(response.at(3));
                int minor = static_cast<unsigned char>(response.at(4));
                int patch = static_cast<unsigned char>(response.at(5));
                firmwareVersion = QString("%1.%2.%3").arg(major).arg(minor).arg(patch);
                emit firmwareVersionReceived(firmwareVersion);
            }
            break;
            
        case CMD_GET_SENSOR:
            if (response.size() >= 4) {
                int sensorType = static_cast<unsigned char>(response.at(3));
                if (sensorType == 1) {
                    sensorModel = "PAW3395";
                } else if (sensorType == 2) {
                    sensorModel = "PAW3950";
                } else {
                    sensorModel = tr("Bilinmeyen Sensör");
                }
                emit sensorModelReceived(sensorModel);
            }
            break;
            
        default:
            // Diğer yanıt tipleri burada işlenebilir
            break;
    }
}