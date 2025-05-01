#ifndef MOUSETAB_H
#define MOUSETAB_H

#include <QWidget>
#include <QTabWidget>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QColorDialog>
#include <QSpinBox>
#include "../usb/devicemanager.h"
#include "../profiles/profilemanager.h"
#include "../controllers/mousecontroller.h"

class MouseTab : public QWidget
{
    Q_OBJECT

public:
    explicit MouseTab(DeviceManager *deviceManager, ProfileManager *profileManager, QWidget *parent = nullptr);
    
    // Cihaz bağlantı durumu değiştiğinde
    void onDeviceConnected(const ConnectedDevice& device);
    void onDeviceDisconnected(const ConnectedDevice& device);

private slots:
    // Profil değiştiğinde
    void onProfileChanged(int index);
    
    // DPI ayarları
    void onDpiLevelChanged(int index);
    void onDpiValueChanged(int value);
    void onDpiColorClicked();
    void onDpiEnabledChanged(bool checked);
    
    // Polling rate
    void onPollingRateChanged(int index);
    
    // RGB ayarları
    void onRgbModeChanged(int index);
    void onLogoColorClicked();
    void onScrollColorClicked();
    void onRgbBrightnessChanged(int value);
    void onRgbSpeedChanged(int value);
    
    // Performans ayarları
    void onLodChanged(int value);
    void onAngleSnappingChanged(bool checked);
    void onMotionSyncChanged(bool checked);
    void onDebounceTimeChanged(double value);
    void onCalibrateClicked();
    
    // Tuş atamaları
    void onButtonSelected(int buttonIndex);
    void onButtonFunctionChanged(int index);
    
    // Profil yönetimi
    void onSaveProfileClicked();
    void onResetToDefaultsClicked();

private:
    // Arayüz kurulumu
    void setupUi();
    void setupDpiTab();
    void setupRgbTab();
    void setupPerformanceTab();
    void setupButtonsTab();
    
    // Arayüzü güncelle
    void updateUiFromProfile();
    void updateMouseView();
    void updateDpiControls();
    
    // Cihaz ve profil yöneticileri
    DeviceManager *deviceManager;
    ProfileManager *profileManager;
    MouseController *mouseController;
    
    // Aktif profil
    QString currentProfile;
    
    // Ana sekmeler
    QTabWidget *tabWidget;
    
    // DPI sekmesi bileşenleri
    QWidget *dpiTab;
    QComboBox *dpiLevelCombo;
    QSpinBox *dpiValueSpin;
    QPushButton *dpiColorButton;
    QCheckBox *dpiEnabledCheck;
    QComboBox *pollingRateCombo;
    QColor currentDpiColor;
    
    // RGB sekmesi bileşenleri
    QWidget *rgbTab;
    QComboBox *rgbModeCombo;
    QPushButton *logoColorButton;
    QPushButton *scrollColorButton;
    QSlider *rgbBrightnessSlider;
    QLabel *rgbBrightnessLabel;
    QSlider *rgbSpeedSlider;
    QLabel *rgbSpeedLabel;
    QColor currentLogoColor;
    QColor currentScrollColor;
    
    // Performans sekmesi bileşenleri
    QWidget *performanceTab;
    QSlider *lodSlider;
    QLabel *lodLabel;
    QCheckBox *angleSnappingCheck;
    QCheckBox *motionSyncCheck;
    QDoubleSpinBox *debounceTimeSpin;
    QPushButton *calibrateButton;
    
    // Tuş sekmesi bileşenleri
    QWidget *buttonsTab;
    QWidget *mouseView;
    QComboBox *buttonFunctionCombo;
    QComboBox *buttonActionCombo;
    QComboBox *macroCombo;
    int selectedButtonIndex;
    
    // Profil bileşenleri
    QComboBox *profileCombo;
    QPushButton *saveProfileButton;
    QPushButton *resetButton;
    
    // Cihaz bağlı mı?
    bool isConnected;
};

#endif // MOUSETAB_H