#include "keyboardcontroller.h"
#include <QDebug>

KeyboardController::KeyboardController(HidDevice* device, QObject *parent)
    : QObject(parent),
      device(device),
      rgbMode(RGB_STATIC),
      rgbColor(Qt::white),
      rgbBrightness(100),
      rgbSpeed(50),
      actuationPoint(1.5),
      rapidTriggerEnabled(false),
      dynamicKeystrokeEnabled(false)
{
    // Cihaz bağlantısını kontrol et
    if (!device || !device->isConnected()) {
        qDebug() << "Klavye kontrolcüsü oluşturulurken cihaz bağlantısı bulunamadı";
        return;
    }
    
    // Firmware bilgisini al
    QByteArray command;
    command.append(static_cast<char>(CMD_GET_FIRMWARE));
    sendCommand(command);
    
    // Varsayılan ayarları yükle
    loadDefaultSettings();
}

KeyboardController::~KeyboardController() {
    // Temizleme işlemleri gerekirse burada yapılabilir
}

bool KeyboardController::setRgbMode(RgbMode mode) {
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

bool KeyboardController::setRgbColor(const QColor& color) {
    QByteArray command;
    command.append(static_cast<char>(CMD_RGB_COLOR));
    command.append(static_cast<char>(color.red()));
    command.append(static_cast<char>(color.green()));
    command.append(static_cast<char>(color.blue()));
    
    if (sendCommand(command)) {
        rgbColor = color;
        emit commandSuccess(tr("RGB rengi başarıyla değiştirildi"));
        return true;
    }
    
    return false;
}

bool KeyboardController::setRgbBrightness(int brightness) {
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

bool KeyboardController::setRgbSpeed(int speed) {
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

bool KeyboardController::setKeyFunction(int keyIndex, const KeyConfig& config) {
    if (keyIndex < 0 || keyIndex > 104) { // Standart klavye tuş sayısı
        emit errorOccurred(tr("Geçersiz tuş indeksi: %1").arg(keyIndex));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_KEY_REMAP));
    command.append(static_cast<char>(keyIndex));
    command.append(static_cast<char>(config.function));
    command.append(static_cast<char>(config.keyCode));
    
    // Makro adı için maksimum 16 karakter
    QByteArray macroNameBytes = config.macroName.toUtf8();
    macroNameBytes.truncate(16);
    command.append(macroNameBytes);
    
    if (sendCommand(command)) {
        keyConfigurations[keyIndex] = config;
        emit commandSuccess(tr("Tuş fonksiyonu başarıyla değiştirildi"));
        return true;
    }
    
    return false;
}

KeyConfig KeyboardController::getKeyFunction(int keyIndex) const {
    if (keyConfigurations.contains(keyIndex)) {
        return keyConfigurations[keyIndex];
    }
    
    // Varsayılan değer
    KeyConfig defaultConfig;
    defaultConfig.function = KEY_DEFAULT;
    defaultConfig.keyCode = 0;
    defaultConfig.macroName = "";
    
    return defaultConfig;
}

bool KeyboardController::setMacro(const QString& name, const QByteArray& macroData) {
    if (name.isEmpty()) {
        emit errorOccurred(tr("Makro adı boş olamaz"));
        return false;
    }
    
    if (macroData.size() > 1024) { // Maksimum makro boyutu
        emit errorOccurred(tr("Makro verisi çok büyük (maksimum 1024 bayt)"));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_MACRO_SET));
    
    // Makro adı için maksimum 16 karakter
    QByteArray nameBytes = name.toUtf8();
    nameBytes.truncate(16);
    command.append(static_cast<char>(nameBytes.size()));
    command.append(nameBytes);
    
    // Makro verisi
    command.append(static_cast<char>(macroData.size() & 0xFF));
    command.append(static_cast<char>((macroData.size() >> 8) & 0xFF));
    command.append(macroData);
    
    if (sendCommand(command)) {
        emit commandSuccess(tr("Makro başarıyla kaydedildi: %1").arg(name));
        return true;
    }
    
    return false;
}

bool KeyboardController::deleteMacro(const QString& name) {
    if (name.isEmpty()) {
        emit errorOccurred(tr("Makro adı boş olamaz"));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_MACRO_CLEAR));
    
    // Makro adı için maksimum 16 karakter
    QByteArray nameBytes = name.toUtf8();
    nameBytes.truncate(16);
    command.append(static_cast<char>(nameBytes.size()));
    command.append(nameBytes);
    
    if (sendCommand(command)) {
        emit commandSuccess(tr("Makro başarıyla silindi: %1").arg(name));
        return true;
    }
    
    return false;
}

bool KeyboardController::setActuationPoint(double distance) {
    if (distance < 0.1 || distance > 4.0) {
        emit errorOccurred(tr("Geçersiz basış mesafesi: %1mm").arg(distance));
        return false;
    }
    
    // Basış mesafesini 0.1mm hassasiyetle gönder (10 katını al)
    int distanceValue = static_cast<int>(distance * 10);
    
    QByteArray command;
    command.append(static_cast<char>(CMD_ACTUATION_POINT));
    command.append(static_cast<char>(distanceValue));
    
    if (sendCommand(command)) {
        actuationPoint = distance;
        emit commandSuccess(tr("Basış mesafesi başarıyla değiştirildi: %1mm").arg(distance));
        return true;
    }
    
    return false;
}

bool KeyboardController::setRapidTrigger(bool enabled) {
    QByteArray command;
    command.append(static_cast<char>(CMD_RAPID_TRIGGER));
    command.append(static_cast<char>(enabled ? 1 : 0));
    
    if (sendCommand(command)) {
        rapidTriggerEnabled = enabled;
        emit commandSuccess(tr("Rapid Trigger %1").arg(enabled ? "etkinleştirildi" : "devre dışı bırakıldı"));
        return true;
    }
    
    return false;
}

bool KeyboardController::setDynamicKeystroke(bool enabled) {
    QByteArray command;
    command.append(static_cast<char>(CMD_DYNAMIC_KEYSTROKE));
    command.append(static_cast<char>(enabled ? 1 : 0));
    
    if (sendCommand(command)) {
        dynamicKeystrokeEnabled = enabled;
        emit commandSuccess(tr("Dynamic Keystroke %1").arg(enabled ? "etkinleştirildi" : "devre dışı bırakıldı"));
        return true;
    }
    
    return false;
}

bool KeyboardController::saveProfile(int profileIndex, const QString& name) {
    if (profileIndex < 0 || profileIndex > 9) { // Maksimum 10 profil
        emit errorOccurred(tr("Geçersiz profil indeksi: %1").arg(profileIndex));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_PROFILE_SAVE));
    command.append(static_cast<char>(profileIndex));
    
    // Profil adı için maksimum 16 karakter
    QByteArray nameBytes = name.toUtf8();
    nameBytes.truncate(16);
    command.append(static_cast<char>(nameBytes.size()));
    command.append(nameBytes);
    
    if (sendCommand(command)) {
        emit commandSuccess(tr("Profil başarıyla kaydedildi: %1").arg(name));
        return true;
    }
    
    return false;
}

bool KeyboardController::loadProfile(int profileIndex) {
    if (profileIndex < 0 || profileIndex > 9) { // Maksimum 10 profil
        emit errorOccurred(tr("Geçersiz profil indeksi: %1").arg(profileIndex));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_PROFILE_LOAD));
    command.append(static_cast<char>(profileIndex));
    
    if (sendCommand(command)) {
        emit commandSuccess(tr("Profil başarıyla yüklendi"));
        return true;
    }
    
    return false;
}

bool KeyboardController::deleteProfile(int profileIndex) {
    if (profileIndex < 0 || profileIndex > 9) { // Maksimum 10 profil
        emit errorOccurred(tr("Geçersiz profil indeksi: %1").arg(profileIndex));
        return false;
    }
    
    QByteArray command;
    command.append(static_cast<char>(CMD_PROFILE_DELETE));
    command.append(static_cast<char>(profileIndex));
    
    if (sendCommand(command)) {
        emit commandSuccess(tr("Profil başarıyla silindi"));
        return true;
    }
    
    return false;
}

bool KeyboardController::resetToDefaults() {
    QByteArray command;
    command.append(static_cast<char>(CMD_RESET));
    
    if (sendCommand(command)) {
        // Varsayılan değerleri yükle
        loadDefaultSettings();
        emit commandSuccess(tr("Klavye ayarları varsayılan değerlere sıfırlandı"));
        return true;
    }
    
    return false;
}

QString KeyboardController::getFirmwareVersion() const {
    return firmwareVersion;
}

bool KeyboardController::sendCommand(const QByteArray &data) {
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
    
    // Yanıtı işle
    handleResponse(response);
    
    return true;
}

void KeyboardController::handleResponse(const QByteArray &response) {
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
            
        default:
            // Diğer yanıt tipleri burada işlenebilir
            break;
    }
}

void KeyboardController::loadDefaultSettings() {
    rgbMode = RGB_STATIC;
    rgbColor = Qt::white;
    rgbBrightness = 100;
    rgbSpeed = 50;
    actuationPoint = 1.5;
    rapidTriggerEnabled = false;
    dynamicKeystrokeEnabled = false;
    
    // Varsayılan tuş atamaları
    keyConfigurations.clear();
}