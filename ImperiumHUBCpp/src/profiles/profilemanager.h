#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <QObject>
#include <QMap>
#include <QColor>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>

enum class ProfileType {
    Mouse,
    Keyboard,
    Headset
};

enum class PollingRate {
    Rate125Hz = 125,
    Rate250Hz = 250,
    Rate500Hz = 500,
    Rate1000Hz = 1000,
    Rate2000Hz = 2000,
    Rate4000Hz = 4000,
    Rate8000Hz = 8000
};

enum class KeyFunction {
    KEY_DEFAULT = 0,
    KEY_DISABLED = 1,
    KEY_KEYBOARD = 2,
    KEY_MOUSE = 3,
    KEY_MEDIA = 4,
    KEY_MACRO = 5,
    KEY_SYSTEM = 6
};

class ProfileManager : public QObject
{
    Q_OBJECT

public:
    explicit ProfileManager(QObject *parent = nullptr);
    ~ProfileManager();
    
    // Profil yönetimi
    bool createProfile(ProfileType type, const QString &name);
    bool deleteProfile(ProfileType type, const QString &name);
    bool renameProfile(ProfileType type, const QString &oldName, const QString &newName);
    QStringList getProfileNames(ProfileType type);
    bool setActiveProfile(ProfileType type, const QString &name);
    QString getActiveProfile(ProfileType type);
    
    // Profil kaydetme/yükleme
    bool saveProfiles();
    bool loadProfiles();
    
    // Fare profil ayarları
    int getMouseDpiLevelCount(const QString &profileName);
    QJsonObject getMouseDpiLevel(const QString &profileName, int level);
    bool setMouseDpiLevel(const QString &profileName, int level, int dpi, const QColor &color, bool enabled);
    int getMousePollingRate(const QString &profileName);
    bool setMousePollingRate(const QString &profileName, int rate);
    int getMouseRgbMode(const QString &profileName);
    bool setMouseRgbMode(const QString &profileName, int mode);
    QColor getMouseLogoColor(const QString &profileName);
    bool setMouseLogoColor(const QString &profileName, const QColor &color);
    QColor getMouseScrollColor(const QString &profileName);
    bool setMouseScrollColor(const QString &profileName, const QColor &color);
    int getMouseRgbBrightness(const QString &profileName);
    bool setMouseRgbBrightness(const QString &profileName, int brightness);
    int getMouseRgbSpeed(const QString &profileName);
    bool setMouseRgbSpeed(const QString &profileName, int speed);
    int getMouseLiftOffDistance(const QString &profileName);
    bool setMouseLiftOffDistance(const QString &profileName, int distance);
    bool getMouseAngleSnapping(const QString &profileName);
    bool setMouseAngleSnapping(const QString &profileName, bool enabled);
    bool getMouseMotionSync(const QString &profileName);
    bool setMouseMotionSync(const QString &profileName, bool enabled);
    double getMouseDebounceTime(const QString &profileName);
    bool setMouseDebounceTime(const QString &profileName, double time);
    QJsonObject getMouseButtonMapping(const QString &profileName, int buttonIndex);
    bool setMouseButtonMapping(const QString &profileName, int buttonIndex, int function, int action, const QString &macroName);
    
    // Klavye profil ayarları
    int getKeyboardRgbMode(const QString &profileName);
    bool setKeyboardRgbMode(const QString &profileName, int mode);
    QColor getKeyboardRgbColor(const QString &profileName);
    bool setKeyboardRgbColor(const QString &profileName, const QColor &color);
    int getKeyboardRgbBrightness(const QString &profileName);
    bool setKeyboardRgbBrightness(const QString &profileName, int brightness);
    int getKeyboardRgbSpeed(const QString &profileName);
    bool setKeyboardRgbSpeed(const QString &profileName, int speed);
    int getKeyboardRgbDirection(const QString &profileName);
    bool setKeyboardRgbDirection(const QString &profileName, int direction);
    int getKeyboardPollingRate(const QString &profileName);
    bool setKeyboardPollingRate(const QString &profileName, int rate);
    int getKeyboardDebounceTime(const QString &profileName);
    bool setKeyboardDebounceTime(const QString &profileName, int time);
    int getKeyboardNkroMode(const QString &profileName);
    bool setKeyboardNkroMode(const QString &profileName, int mode);
    bool getKeyboardGameMode(const QString &profileName);
    bool setKeyboardGameMode(const QString &profileName, bool enabled);
    bool getKeyboardWinKeyLock(const QString &profileName);
    bool setKeyboardWinKeyLock(const QString &profileName, bool enabled);
    double getKeyboardActuationPoint(const QString &profileName);
    bool setKeyboardActuationPoint(const QString &profileName, double point);
    bool getKeyboardRapidTrigger(const QString &profileName);
    bool setKeyboardRapidTrigger(const QString &profileName, bool enabled);
    bool getKeyboardDynamicKeystroke(const QString &profileName);
    bool setKeyboardDynamicKeystroke(const QString &profileName, bool enabled);
    QJsonObject getKeyboardKeyMapping(const QString &profileName, int keyIndex);
    bool setKeyboardKeyMapping(const QString &profileName, int keyIndex, KeyFunction function, int keyCode, const QString &macroName);
    
    // Makro yönetimi
    QStringList getMacroNames();
    bool createMacro(const QString &name);
    bool deleteMacro(const QString &name);
    bool renameMacro(const QString &oldName, const QString &newName);
    QJsonArray getMacroSteps(const QString &name);
    bool addMacroStep(const QString &name, const QJsonObject &step);
    bool updateMacroStep(const QString &name, int index, const QJsonObject &step);
    bool deleteMacroStep(const QString &name, int index);
    bool moveMacroStep(const QString &name, int fromIndex, int toIndex);
    QJsonObject getMacroSettings(const QString &name);
    bool setMacroSettings(const QString &name, const QJsonObject &settings);

private:
    // Profil verileri
    QJsonObject profiles;
    QJsonObject macros;
    
    // Aktif profiller
    QMap<ProfileType, QString> activeProfiles;
    
    // Profil dosya yolları
    QString getProfilesFilePath();
    QString getMacrosFilePath();
    
    // Varsayılan profil oluştur
    QJsonObject createDefaultMouseProfile();
    QJsonObject createDefaultKeyboardProfile();
    QJsonObject createDefaultHeadsetProfile();
    
    // Profil türünü string'e dönüştür
    QString profileTypeToString(ProfileType type);
};

#endif // PROFILEMANAGER_H