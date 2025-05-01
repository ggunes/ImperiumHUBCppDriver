#include "mainwindow.h"
#include "mousetab.h"
#include "keyboardtab.h"
#include "settingstab.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      deviceManager(new DeviceManager(this)),
      profileManager(new ProfileManager(this)),
      trayIcon(nullptr)
{
    setupUi();
    setupTrayIcon();
    
    // Cihazları algıla
    detectDevices();
    
    // Ayarları yükle
    loadSettings();
    
    // Pencere ayarları
    setWindowTitle("ImperiumHUB");
    setMinimumSize(800, 600);
    
    // Durum çubuğu
    statusBar()->showMessage(tr("Hazır"));
}

MainWindow::~MainWindow()
{
    // Ayarları kaydet
    saveSettings();
}

void MainWindow::setupUi()
{
    // Ana widget
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Ana düzen
    mainLayout = new QVBoxLayout(centralWidget);
    
    // Cihaz seçimi
    QHBoxLayout *deviceLayout = new QHBoxLayout();
    
    deviceLayout->addWidget(new QLabel(tr("Cihaz:"), this));
    
    deviceCombo = new QComboBox(this);
    deviceLayout->addWidget(deviceCombo, 1);
    
    refreshButton = new QPushButton(tr("Yenile"), this);
    deviceLayout->addWidget(refreshButton);
    
    mainLayout->addLayout(deviceLayout);
    
    // Sekmeler
    tabWidget = new QTabWidget(this);
    mainLayout->addWidget(tabWidget);
    
    // Sekmeleri oluştur
    mouseTab = new MouseTab(deviceManager, profileManager, this);
    keyboardTab = new KeyboardTab(deviceManager, profileManager, this);
    settingsTab = new SettingsTab(this);
    
    tabWidget->addTab(mouseTab, tr("Fare"));
    tabWidget->addTab(keyboardTab, tr("Klavye"));
    tabWidget->addTab(settingsTab, tr("Ayarlar"));
    
    // Menüleri oluştur
    createMenus();
    
    // Bağlantıları kur
    connect(deviceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onDeviceChanged);
    connect(refreshButton, &QPushButton::clicked,
            this, &MainWindow::detectDevices);
    connect(settingsTab, &SettingsTab::settingsChanged,
            this, &MainWindow::applySettings);
}

void MainWindow::createMenus()
{
    // Dosya menüsü
    QMenu *fileMenu = menuBar()->addMenu(tr("Dosya"));
    
    QAction *importAction = fileMenu->addAction(tr("Profil İçe Aktar"));
    connect(importAction, &QAction::triggered, this, &MainWindow::importProfile);
    
    QAction *exportAction = fileMenu->addAction(tr("Profil Dışa Aktar"));
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportProfile);
    
    fileMenu->addSeparator();
    
    QAction *firmwareAction = fileMenu->addAction(tr("Yazılım Güncelleme"));
    connect(firmwareAction, &QAction::triggered, this, &MainWindow::updateFirmware);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction(tr("Çıkış"));
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
    
    // Yardım menüsü
    QMenu *helpMenu = menuBar()->addMenu(tr("Yardım"));
    
    QAction *checkUpdateAction = helpMenu->addAction(tr("Güncellemeleri Kontrol Et"));
    connect(checkUpdateAction, &QAction::triggered, this, &MainWindow::checkForUpdates);
    
    QAction *docsAction = helpMenu->addAction(tr("Dokümantasyon"));
    connect(docsAction, &QAction::triggered, this, &MainWindow::openDocumentation);
    
    QAction *aboutAction = helpMenu->addAction(tr("Hakkında"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
}

void MainWindow::setupTrayIcon()
{
    // Sistem tepsisi simgesi oluştur
    trayIcon = new QSystemTrayIcon(QIcon(":/icons/app_icon.png"), this);
    
    // Menü oluştur
    QMenu *trayMenu = new QMenu(this);
    
    QAction *showAction = new QAction(tr("Göster"), this);
    connect(showAction, &QAction::triggered, this, &MainWindow::show);
    trayMenu->addAction(showAction);
    
    QAction *hideAction = new QAction(tr("Gizle"), this);
    connect(hideAction, &QAction::triggered, this, &MainWindow::hide);
    trayMenu->addAction(hideAction);
    
    trayMenu->addSeparator();
    
    QAction *exitAction = new QAction(tr("Çıkış"), this);
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);
    trayMenu->addAction(exitAction);
    
    // Menüyü simgeye bağla
    trayIcon->setContextMenu(trayMenu);
    
    // Çift tıklama olayı
    connect(trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            if (isVisible()) {
                hide();
            } else {
                show();
                activateWindow();
            }
        }
    });
    
    // Simgeyi göster
    trayIcon->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    bool minimizeToTray = settings.value("minimizeToTray", false).toBool();
    
    if (minimizeToTray && trayIcon && trayIcon->isVisible()) {
        // Pencereyi kapat yerine gizle
        hide();
        event->ignore();
        
        // Bildirim göster
        trayIcon->showMessage(tr("ImperiumHUB"), 
                             tr("Uygulama sistem tepsisinde çalışmaya devam ediyor."),
                             QSystemTrayIcon::Information, 
                             2000);
    } else {
        // Ayarları kaydet
        saveSettings();
        event->accept();
    }
}

void MainWindow::detectDevices()
{
    // Mevcut seçimi hatırla
    QString currentDeviceId;
    if (deviceCombo->currentIndex() >= 0) {
        currentDeviceId = deviceCombo->currentData().toString();
    }
    
    // Cihaz listesini temizle
    deviceCombo->clear();
    
    // Cihazları algıla
    QList<DeviceInfo> devices = deviceManager->detectDevices();
    
    // Cihazları listeye ekle
    for (const DeviceInfo &device : devices) {
        deviceCombo->addItem(device.name, device.id);
    }
    
    // Önceki seçimi geri yükle
    if (!currentDeviceId.isEmpty()) {
        int index = deviceCombo->findData(currentDeviceId);
        if (index >= 0) {
            deviceCombo->setCurrentIndex(index);
        }
    }
    
    // Durum çubuğunu güncelle
    statusBar()->showMessage(tr("%1 cihaz bulundu").arg(devices.size()));
}

void MainWindow::onDeviceChanged(int index)
{
    if (index < 0) {
        // Cihaz seçilmedi
        mouseTab->setEnabled(false);
        keyboardTab->setEnabled(false);
        return;
    }
    
    QString deviceId = deviceCombo->currentData().toString();
    DeviceInfo device = deviceManager->getDeviceInfo(deviceId);
    
    // Cihaz türüne göre sekmeleri etkinleştir
    if (device.type == DeviceType::Mouse) {
        mouseTab->setEnabled(true);
        mouseTab->setDevice(deviceId);
        keyboardTab->setEnabled(false);
    } else if (device.type == DeviceType::Keyboard) {
        mouseTab->setEnabled(false);
        keyboardTab->setEnabled(true);
        keyboardTab->setDevice(deviceId);
    } else if (device.type == DeviceType::Both) {
        mouseTab->setEnabled(true);
        mouseTab->setDevice(deviceId);
        keyboardTab->setEnabled(true);
        keyboardTab->setDevice(deviceId);
    }
    
    // Durum çubuğunu güncelle
    statusBar()->showMessage(tr("Cihaz bağlandı: %1").arg(device.name));
}

void MainWindow::importProfile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Profil İçe Aktar"), "", tr("Profil Dosyaları (*.json)"));
    
    if (fileName.isEmpty()) {
        return;
    }
    
    if (profileManager->importProfile(fileName)) {
        QMessageBox::information(this, tr("Başarılı"),
                                tr("Profil başarıyla içe aktarıldı."));
        
        // Profil listelerini güncelle
        mouseTab->updateProfileList();
        keyboardTab->updateProfileList();
    } else {
        QMessageBox::warning(this, tr("Hata"),
                            tr("Profil içe aktarılamadı."));
    }
}

void MainWindow::exportProfile()
{
    // Aktif profili al
    QString mouseProfile = profileManager->getActiveProfile(ProfileType::Mouse);
    QString keyboardProfile = profileManager->getActiveProfile(ProfileType::Keyboard);
    
    // Profil seçimi diyaloğu
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Profil Dışa Aktar"));
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QRadioButton *mouseRadio = new QRadioButton(tr("Fare Profili: %1").arg(mouseProfile), &dialog);
    QRadioButton *keyboardRadio = new QRadioButton(tr("Klavye Profili: %1").arg(keyboardProfile), &dialog);
    
    mouseRadio->setChecked(true);
    
    layout->addWidget(mouseRadio);
    layout->addWidget(keyboardRadio);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    
    // Dışa aktarılacak profili belirle
    QString profileName;
    ProfileType profileType;
    
    if (mouseRadio->isChecked()) {
        profileName = mouseProfile;
        profileType = ProfileType::Mouse;
    } else {
        profileName = keyboardProfile;
        profileType = ProfileType::Keyboard;
    }
    
    // Dosya adı seç
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Profil Dışa Aktar"), profileName + ".json", tr("Profil Dosyaları (*.json)"));
    
    if (fileName.isEmpty()) {
        return;
    }
    
    if (profileManager->exportProfile(profileType, profileName, fileName)) {
        QMessageBox::information(this, tr("Başarılı"),
                                tr("Profil başarıyla dışa aktarıldı."));
    } else {
        QMessageBox::warning(this, tr("Hata"),
                            tr("Profil dışa aktarılamadı."));
    }
}

void MainWindow::updateFirmware()
{
    // Cihaz seçili değilse uyar
    if (deviceCombo->currentIndex() < 0) {
        QMessageBox::warning(this, tr("Hata"),
                            tr("Lütfen önce bir cihaz seçin."));
        return;
    }
    
    // Firmware dosyasını seç
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Firmware Dosyası Seç"), "", tr("Firmware Dosyaları (*.bin *.hex)"));
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // Onay diyaloğu
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Firmware Güncelleme"),
                                 tr("Firmware güncellemesi başlatılacak. Cihazınızı güncelleme sırasında çıkarmayın. Devam etmek istiyor musunuz?"),
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply != QMessageBox::Yes) {
        return;
    }
    
    // Firmware güncelleme işlemi
    QString deviceId = deviceCombo->currentData().toString();
    
    QProgressDialog progress(tr("Firmware güncelleniyor..."), tr("İptal"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    
    // İlerleme güncellemesi için bağlantı
    connect(deviceManager, &DeviceManager::firmwareUpdateProgress,
            &progress, &QProgressDialog::setValue);
    
    // Güncellemeyi başlat
    bool success = deviceManager->updateFirmware(deviceId, fileName);
    
    // Bağlantıyı kaldır
    disconnect(deviceManager, &DeviceManager::firmwareUpdateProgress,
               &progress, &QProgressDialog::setValue);
    
    if (success) {
        QMessageBox::information(this, tr("Başarılı"),
                                tr("Firmware başarıyla güncellendi. Cihazı yeniden bağlayın."));
        
        // Cihazları yeniden algıla
        detectDevices();
    } else {
        QMessageBox::warning(this, tr("Hata"),
                            tr("Firmware güncellenemedi."));
    }
}

void MainWindow::checkForUpdates()
{
    // Güncelleme kontrolü
    QProgressDialog progress(tr("Güncellemeler kontrol ediliyor..."), tr("İptal"), 0, 0, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    
    // Burada güncelleme kontrolü yapılacak
    // Şimdilik basit bir simülasyon
    QTimer::singleShot(2000, &progress, &QProgressDialog::close);
    QTimer::singleShot(2100, this, [this]() {
        QMessageBox::information(this, tr("Güncelleme Kontrolü"),
                                tr("Uygulamanız güncel."));
    });
}

void MainWindow::openDocumentation()
{
    // Dokümantasyon sayfasını aç
    QDesktopServices::openUrl(QUrl("https://imperiumgear.com/docs"));
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(this, tr("Hakkında"),
                      tr("<h3>ImperiumHUB</h3>"
                         "<p>Sürüm: 1.0.0</p>"
                         "<p>ImperiumGear cihazları için kontrol yazılımı.</p>"
                         "<p>Copyright © 2023 ImperiumGear. Tüm hakları saklıdır.</p>"));
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        if (isVisible()) {
            hide();
        } else {
            show();
            activateWindow();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Ayarları kontrol et
    QSettings settings;
    bool minimizeToTray = settings.value("minimizeToTray", false).toBool();
    
    if (minimizeToTray && trayIcon && trayIcon->isVisible()) {
        // Sistem tepsisine küçült
        hide();
        event->ignore();
    } else {
        // Normal kapatma
        event->accept();
    }
}

void MainWindow::loadSettings()
{
    QSettings settings;
    
    // Pencere geometrisi
    restoreGeometry(settings.value("windowGeometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    
    // Ayarları uygula
    bool startMinimized = settings.value("startMinimized", false).toBool();
    if (startMinimized) {
        QTimer::singleShot(0, this, &MainWindow::hide);
    }
    
    // Dil ayarı
    QString language = settings.value("language", "tr").toString();
    settingsTab->setLanguage(language);
    
    // Tema ayarı
    QString theme = settings.value("theme", "system").toString();
    settingsTab->setTheme(theme);
    applyTheme(theme);
}

void MainWindow::saveSettings()
{
    QSettings settings;
    
    // Pencere geometrisi
    settings.setValue("windowGeometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::applySettings()
{
    QSettings settings;
    
    // Dil ayarı
    QString language = settings.value("language", "tr").toString();
    // Dil değişikliği için uygulama yeniden başlatılmalı
    
    // Tema ayarı
    QString theme = settings.value("theme", "system").toString();
    applyTheme(theme);
}

void MainWindow::applyTheme(const QString &theme)
{
    if (theme == "light") {
        qApp->setStyle("Fusion");
        QPalette palette;
        qApp->setPalette(palette);
    } else if (theme == "dark") {
        qApp->setStyle("Fusion");
        QPalette palette;
        palette.setColor(QPalette::Window, QColor(53, 53, 53));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(25, 25, 25));
        palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        palette.setColor(QPalette::ToolTipBase, Qt::white);
        palette.setColor(QPalette::ToolTipText, Qt::white);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor(53, 53, 53));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::HighlightedText, Qt::black);
        qApp->setPalette(palette);
    } else {
        // Sistem teması
        qApp->setStyle("");
        QPalette palette;
        qApp->setPalette(palette);
    }
}