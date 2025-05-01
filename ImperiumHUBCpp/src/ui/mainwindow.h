#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include "../usb/devicemanager.h"
#include "../profiles/profilemanager.h"
#include "devicetab.h"
#include "keyboardtab.h"
#include "mousetab.h"
#include "settingstab.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Cihaz bağlantı durumu değiştiğinde
    void onDeviceConnected(const ConnectedDevice& device);
    void onDeviceDisconnected(const ConnectedDevice& device);
    
    // Sistem tepsisi eylemleri
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowHideActionTriggered();
    void onQuitActionTriggered();
    
    // Menü eylemleri
    void onAboutActionTriggered();
    void onCheckForUpdatesActionTriggered();
    void onSettingsActionTriggered();
    
    // Sekme değiştiğinde
    void onTabChanged(int index);

private:
    // Arayüz kurulumu
    void setupUi();
    void setupMenuBar();
    void setupStatusBar();
    void setupSystemTray();
    
    // Cihaz ve profil yöneticileri
    DeviceManager *deviceManager;
    ProfileManager *profileManager;
    
    // Ana sekmeler
    QTabWidget *tabWidget;
    DeviceTab *deviceTab;
    KeyboardTab *keyboardTab;
    MouseTab *mouseTab;
    SettingsTab *settingsTab;
    
    // Durum çubuğu
    QLabel *statusLabel;
    
    // Sistem tepsisi
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *showHideAction;
    QAction *quitAction;
    
    // Menü eylemleri
    QAction *aboutAction;
    QAction *checkForUpdatesAction;
    QAction *settingsAction;
    
    // Pencere durumu
    bool isMinimizedToTray;
};

#endif // MAINWINDOW_H