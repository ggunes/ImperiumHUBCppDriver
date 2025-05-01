#ifndef KEYBOARDTAB_H
#define KEYBOARDTAB_H

#include <QWidget>
#include <QTabWidget>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QColorDialog>
#include <QSpinBox>
#include <QGridLayout>
#include "../usb/devicemanager.h"
#include "../profiles/profilemanager.h"
#include "../controllers/keyboardcontroller.h"

class KeyboardView;
class MacroEditor;

class KeyboardTab : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardTab(DeviceManager *deviceManager, ProfileManager *profileManager, QWidget *parent = nullptr);
    
    // Cihaz bağlantı durumu değiştiğinde
    void onDeviceConnected(const ConnectedDevice& device);
    void onDeviceDisconnected(const ConnectedDevice& device);

private slots:
    // Profil değiştiğinde
    void onProfileChanged(int index);
    
    // RGB ayarları
    void onRgbModeChanged(int index);
    void onRgbBrightnessChanged(int value);
    void onRgbSpeedChanged(int value);
    void onRgbDirectionChanged(int index);
    void onRgbColorClicked();
    
    // Tuş atamaları
    void onKeySelected(int keyCode);
    void onKeyFunctionChanged(int index);
    void onKeyActionChanged(int index);
    void onMacroSelected(const QString& macroName);
    void onEditMacroClicked();
    
    // Performans ayarları
    void onPollingRateChanged(int index);
    void onDebounceTimeChanged(int value);
    void onNkroModeChanged(int index);
    void onGameModeChanged(bool checked);
    void onWinKeyLockChanged(bool checked);
    
    // Profil yönetimi
    void onSaveProfileClicked();
    void onResetToDefaultsClicked();

private:
    // Arayüz kurulumu
    void setupUi();
    void setupRgbTab();
    void setupKeysTab();
    void setupPerformanceTab();
    
    // Arayüzü güncelle
    void updateUiFromProfile();
    void updateKeyboardView();
    void updateKeyControls();
    
    // Cihaz ve profil yöneticileri
    DeviceManager *deviceManager;
    ProfileManager *profileManager;
    KeyboardController *keyboardController;
    
    // Aktif profil
    QString currentProfile;
    
    // Ana sekmeler
    QTabWidget *tabWidget;
    
    // RGB sekmesi bileşenleri
    QWidget *rgbTab;
    QComboBox *rgbModeCombo;
    QSlider *rgbBrightnessSlider;
    QLabel *rgbBrightnessLabel;
    QSlider *rgbSpeedSlider;
    QLabel *rgbSpeedLabel;
    QComboBox *rgbDirectionCombo;
    QPushButton *rgbColorButton;
    QColor currentRgbColor;
    
    // Tuş sekmesi bileşenleri
    QWidget *keysTab;
    KeyboardView *keyboardView;
    QComboBox *keyFunctionCombo;
    QComboBox *keyActionCombo;
    QComboBox *macroCombo;
    QPushButton *editMacroButton;
    int selectedKeyCode;
    
    // Performans sekmesi bileşenleri
    QWidget *performanceTab;
    QComboBox *pollingRateCombo;
    QSlider *debounceTimeSlider;
    QLabel *debounceTimeLabel;
    QComboBox *nkroModeCombo;
    QCheckBox *gameModeCheck;
    QCheckBox *winKeyLockCheck;
    
    // Profil bileşenleri
    QComboBox *profileCombo;
    QPushButton *saveProfileButton;
    QPushButton *resetButton;
    
    // Makro editörü
    MacroEditor *macroEditor;
    
    // Cihaz bağlı mı?
    bool isConnected;
};

#endif // KEYBOARDTAB_H