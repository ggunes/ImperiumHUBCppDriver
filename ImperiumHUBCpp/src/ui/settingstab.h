#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

#include <QWidget>
#include <QTabWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

class SettingsTab : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsTab(QWidget *parent = nullptr);

private slots:
    // Genel ayarlar
    void onStartWithWindowsChanged(bool checked);
    void onMinimizeToTrayChanged(bool checked);
    void onCheckForUpdatesChanged(bool checked);
    
    // Görünüm ayarları
    void onLanguageChanged(int index);
    void onThemeChanged(int index);
    
    // Bildirim ayarları
    void onShowNotificationsChanged(bool checked);
    void onNotificationDurationChanged(int value);
    
    // Kaydetme ve sıfırlama
    void onSaveSettingsClicked();
    void onResetSettingsClicked();

private:
    // Arayüz kurulumu
    void setupUi();
    void setupGeneralTab();
    void setupAppearanceTab();
    void setupNotificationsTab();
    
    // Ayarları yükle
    void loadSettings();
    
    // Ana sekmeler
    QTabWidget *tabWidget;
    
    // Genel ayarlar
    QWidget *generalTab;
    QCheckBox *startWithWindowsCheck;
    QCheckBox *minimizeToTrayCheck;
    QCheckBox *checkForUpdatesCheck;
    
    // Görünüm ayarları
    QWidget *appearanceTab;
    QComboBox *languageCombo;
    QComboBox *themeCombo;
    
    // Bildirim ayarları
    QWidget *notificationsTab;
    QCheckBox *showNotificationsCheck;
    QSpinBox *notificationDurationSpin;
    
    // Butonlar
    QPushButton *saveButton;
    QPushButton *resetButton;
};

#endif // SETTINGSTAB_H