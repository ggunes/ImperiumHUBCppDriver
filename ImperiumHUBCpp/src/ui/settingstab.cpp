#include "settingstab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QTranslator>
#include <QApplication>

SettingsTab::SettingsTab(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    loadSettings();
}

void SettingsTab::setupUi()
{
    // Ana düzen
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Uygulama ayarları
    QGroupBox *appSettingsGroup = new QGroupBox(tr("Uygulama Ayarları"), this);
    QGridLayout *appSettingsLayout = new QGridLayout(appSettingsGroup);
    
    // Dil seçimi
    appSettingsLayout->addWidget(new QLabel(tr("Dil:"), this), 0, 0);
    languageCombo = new QComboBox(this);
    languageCombo->addItem("Türkçe", "tr");
    languageCombo->addItem("English", "en");
    appSettingsLayout->addWidget(languageCombo, 0, 1);
    
    // Tema seçimi
    appSettingsLayout->addWidget(new QLabel(tr("Tema:"), this), 1, 0);
    themeCombo = new QComboBox(this);
    themeCombo->addItem(tr("Açık"), "light");
    themeCombo->addItem(tr("Koyu"), "dark");
    themeCombo->addItem(tr("Sistem"), "system");
    appSettingsLayout->addWidget(themeCombo, 1, 1);
    
    // Başlangıç ayarları
    startWithWindowsCheck = new QCheckBox(tr("Windows ile Başlat"), this);
    appSettingsLayout->addWidget(startWithWindowsCheck, 2, 0, 1, 2);
    
    startMinimizedCheck = new QCheckBox(tr("Küçültülmüş Başlat"), this);
    appSettingsLayout->addWidget(startMinimizedCheck, 3, 0, 1, 2);
    
    minimizeToTrayCheck = new QCheckBox(tr("Sistem Tepsisine Küçült"), this);
    appSettingsLayout->addWidget(minimizeToTrayCheck, 4, 0, 1, 2);
    
    checkUpdatesCheck = new QCheckBox(tr("Başlangıçta Güncellemeleri Kontrol Et"), this);
    appSettingsLayout->addWidget(checkUpdatesCheck, 5, 0, 1, 2);
    
    mainLayout->addWidget(appSettingsGroup);
    
    // Cihaz ayarları
    QGroupBox *deviceSettingsGroup = new QGroupBox(tr("Cihaz Ayarları"), this);
    QVBoxLayout *deviceSettingsLayout = new QVBoxLayout(deviceSettingsGroup);
    
    autoConnectCheck = new QCheckBox(tr("Cihazları Otomatik Bağla"), this);
    deviceSettingsLayout->addWidget(autoConnectCheck);
    
    loadLastProfileCheck = new QCheckBox(tr("Bağlanırken Son Profili Yükle"), this);
    deviceSettingsLayout->addWidget(loadLastProfileCheck);
    
    mainLayout->addWidget(deviceSettingsGroup);
    
    // Butonlar
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    applyButton = new QPushButton(tr("Uygula"), this);
    buttonLayout->addWidget(applyButton);
    
    resetButton = new QPushButton(tr("Sıfırla"), this);
    buttonLayout->addWidget(resetButton);
    
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    
    // Bağlantıları kur
    connect(applyButton, &QPushButton::clicked, this, &SettingsTab::saveSettings);
    connect(resetButton, &QPushButton::clicked, this, &SettingsTab::loadSettings);
    
    // Değişiklik sinyalleri
    connect(languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsTab::onSettingChanged);
    connect(themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsTab::onSettingChanged);
    connect(startWithWindowsCheck, &QCheckBox::toggled, this, &SettingsTab::onSettingChanged);
    connect(startMinimizedCheck, &QCheckBox::toggled, this, &SettingsTab::onSettingChanged);
    connect(minimizeToTrayCheck, &QCheckBox::toggled, this, &SettingsTab::onSettingChanged);
    connect(checkUpdatesCheck, &QCheckBox::toggled, this, &SettingsTab::onSettingChanged);
    connect(autoConnectCheck, &QCheckBox::toggled, this, &SettingsTab::onSettingChanged);
    connect(loadLastProfileCheck, &QCheckBox::toggled, this, &SettingsTab::onSettingChanged);
}

void SettingsTab::loadSettings()
{
    QSettings settings;
    
    // Dil ayarı
    QString language = settings.value("language", "tr").toString();
    int langIndex = languageCombo->findData(language);
    if (langIndex >= 0) {
        languageCombo->setCurrentIndex(langIndex);
    }
    
    // Tema ayarı
    QString theme = settings.value("theme", "system").toString();
    int themeIndex = themeCombo->findData(theme);
    if (themeIndex >= 0) {
        themeCombo->setCurrentIndex(themeIndex);
    }
    
    // Başlangıç ayarları
    startWithWindowsCheck->setChecked(settings.value("startWithWindows", false).toBool());
    startMinimizedCheck->setChecked(settings.value("startMinimized", false).toBool());
    minimizeToTrayCheck->setChecked(settings.value("minimizeToTray", false).toBool());
    checkUpdatesCheck->setChecked(settings.value("checkUpdatesOnStartup", true).toBool());
    
    // Cihaz ayarları
    autoConnectCheck->setChecked(settings.value("autoConnectDevices", true).toBool());
    loadLastProfileCheck->setChecked(settings.value("loadLastProfile", true).toBool());
    
    // Değişiklik bayrağını sıfırla
    settingsChanged = false;
}

void SettingsTab::saveSettings()
{
    QSettings settings;
    
    // Dil ayarı
    QString language = languageCombo->currentData().toString();
    settings.setValue("language", language);
    
    // Tema ayarı
    QString theme = themeCombo->currentData().toString();
    settings.setValue("theme", theme);
    
    // Başlangıç ayarları
    settings.setValue("startWithWindows", startWithWindowsCheck->isChecked());
    settings.setValue("startMinimized", startMinimizedCheck->isChecked());
    settings.setValue("minimizeToTray", minimizeToTrayCheck->isChecked());
    settings.setValue("checkUpdatesOnStartup", checkUpdatesCheck->isChecked());
    
    // Cihaz ayarları
    settings.setValue("autoConnectDevices", autoConnectCheck->isChecked());
    settings.setValue("loadLastProfile", loadLastProfileCheck->isChecked());
    
    // Windows başlangıç ayarı
    setStartWithWindows(startWithWindowsCheck->isChecked());
    
    // Değişiklik sinyali gönder
    if (settingsChanged) {
        emit settingsChanged();
        settingsChanged = false;
    }
    
    QMessageBox::information(this, tr("Ayarlar"),
                            tr("Ayarlar başarıyla kaydedildi."));
}

void SettingsTab::setLanguage(const QString &language)
{
    int index = languageCombo->findData(language);
    if (index >= 0) {
        languageCombo->setCurrentIndex(index);
    }
}

void SettingsTab::setTheme(const QString &theme)
{
    int index = themeCombo->findData(theme);
    if (index >= 0) {
        themeCombo->setCurrentIndex(index);
    }
}

void SettingsTab::onSettingChanged()
{
    settingsChanged = true;
}

void SettingsTab::setStartWithWindows(bool enable)
{
    // Windows başlangıç kaydı
    QSettings registry("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    
    if (enable) {
        QString appPath = QCoreApplication::applicationFilePath();
        registry.setValue("ImperiumHUB", appPath.replace('/', '\\'));
    } else {
        registry.remove("ImperiumHUB");
    }
}
#include "settingstab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QSettings>

SettingsTab::SettingsTab(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    loadSettings();
}

void SettingsTab::setupUi()
{
    // Ana düzen
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Sekmeler
    tabWidget = new QTabWidget(this);
    
    // Sekmeleri oluştur
    setupGeneralTab();
    setupAppearanceTab();
    setupNotificationsTab();
    
    mainLayout->addWidget(tabWidget);
    
    // Butonlar
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    saveButton = new QPushButton(tr("Ayarları Kaydet"), this);
    buttonLayout->addWidget(saveButton);
    
    resetButton = new QPushButton(tr("Varsayılana Sıfırla"), this);
    buttonLayout->addWidget(resetButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Bağlantıları kur
    connect(startWithWindowsCheck, &QCheckBox::toggled,
            this, &SettingsTab::onStartWithWindowsChanged);
    connect(minimizeToTrayCheck, &QCheckBox::toggled,
            this, &SettingsTab::onMinimizeToTrayChanged);
    connect(checkForUpdatesCheck, &QCheckBox::toggled,
            this, &SettingsTab::onCheckForUpdatesChanged);
    
    connect(languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsTab::onLanguageChanged);
    connect(themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsTab::onThemeChanged);
    
    connect(showNotificationsCheck, &QCheckBox::toggled,
            this, &SettingsTab::onShowNotificationsChanged);
    connect(notificationDurationSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsTab::onNotificationDurationChanged);
    
    connect(saveButton, &QPushButton::clicked,
            this, &SettingsTab::onSaveSettingsClicked);
    connect(resetButton, &QPushButton::clicked,
            this, &SettingsTab::onResetSettingsClicked);
}

void SettingsTab::setupGeneralTab()
{
    generalTab = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(generalTab);
    
    // Başlangıç ayarları
    QGroupBox *startupGroup = new QGroupBox(tr("Başlangıç"), generalTab);
    QVBoxLayout *startupLayout = new QVBoxLayout(startupGroup);
    
    startWithWindowsCheck = new QCheckBox(tr("Windows ile başlat"), generalTab);
    startupLayout->addWidget(startWithWindowsCheck);
    
    minimizeToTrayCheck = new QCheckBox(tr("Kapatıldığında sistem tepsisine küçült"), generalTab);
    startupLayout->addWidget(minimizeToTrayCheck);
    
    layout->addWidget(startupGroup);
    
    // Güncelleme ayarları
    QGroupBox *updateGroup = new QGroupBox(tr("Güncellemeler"), generalTab);
    QVBoxLayout *updateLayout = new QVBoxLayout(updateGroup);
    
    checkForUpdatesCheck = new QCheckBox(tr("Başlangıçta güncellemeleri kontrol et"), generalTab);
    updateLayout->addWidget(checkForUpdatesCheck);
    
    layout->addWidget(updateGroup);
    
    layout->addStretch();
    
    tabWidget->addTab(generalTab, tr("Genel"));
}

void SettingsTab::setupAppearanceTab()
{
    appearanceTab = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(appearanceTab);
    
    // Dil ayarları
    QGroupBox *languageGroup = new QGroupBox(tr("Dil"), appearanceTab);
    QHBoxLayout *languageLayout = new QHBoxLayout(languageGroup);
    
    languageLayout->addWidget(new QLabel(tr("Uygulama dili:"), appearanceTab));
    
    languageCombo = new QComboBox(appearanceTab);
    languageCombo->addItem(tr("Türkçe"), "tr");
    languageCombo->addItem(tr("İngilizce"), "en");
    languageLayout->addWidget(languageCombo);
    
    layout->addWidget(languageGroup);
    
    // Tema ayarları
    QGroupBox *themeGroup = new QGroupBox(tr("Tema"), appearanceTab);
    QHBoxLayout *themeLayout = new QHBoxLayout(themeGroup);
    
    themeLayout->addWidget(new QLabel(tr("Uygulama teması:"), appearanceTab));
    
    themeCombo = new QComboBox(appearanceTab);
    themeCombo->addItem(tr("Açık"), "light");
    themeCombo->addItem(tr("Koyu"), "dark");
    themeCombo->addItem(tr("Sistem"), "system");
    themeLayout->addWidget(themeCombo);
    
    layout->addWidget(themeGroup);
    
    layout->addStretch();
    
    tabWidget->addTab(appearanceTab, tr("Görünüm"));
}

void SettingsTab::setupNotificationsTab()
{
    notificationsTab = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(notificationsTab);
    
    // Bildirim ayarları
    QGroupBox *notificationGroup = new QGroupBox(tr("Bildirimler"), notificationsTab);
    QVBoxLayout *notificationLayout = new QVBoxLayout(notificationGroup);
    
    showNotificationsCheck = new QCheckBox(tr("Bildirimleri göster"), notificationsTab);
    notificationLayout->addWidget(showNotificationsCheck);
    
    QHBoxLayout *durationLayout = new QHBoxLayout();
    durationLayout->addWidget(new QLabel(tr("Bildirim süresi (saniye):"), notificationsTab));
    
    notificationDurationSpin = new QSpinBox(notificationsTab);
    notificationDurationSpin->setRange(1, 10);
    notificationDurationSpin->setValue(5);
    durationLayout->addWidget(notificationDurationSpin);
    
    notificationLayout->addLayout(durationLayout);
    
    layout->addWidget(notificationGroup);
    
    layout->addStretch();
    
    tabWidget->addTab(notificationsTab, tr("Bildirimler"));
}

void SettingsTab::loadSettings()
{
    QSettings settings;
    
    // Genel ayarlar
    startWithWindowsCheck->setChecked(settings.value("general/startWithWindows", false).toBool());
    minimizeToTrayCheck->setChecked(settings.value("general/minimizeToTray", true).toBool());
    checkForUpdatesCheck->setChecked(settings.value("general/checkForUpdates", true).toBool());
    
    // Görünüm ayarları
    QString language = settings.value("appearance/language", "tr").toString();
    int languageIndex = languageCombo->findData(language);
    if (languageIndex >= 0) {
        languageCombo->setCurrentIndex(languageIndex);
    }
    
    QString theme = settings.value("appearance/theme", "system").toString();
    int themeIndex = themeCombo->findData(theme);
    if (themeIndex >= 0) {
        themeCombo->setCurrentIndex(themeIndex);
    }
    
    // Bildirim ayarları
    showNotificationsCheck->setChecked(settings.value("notifications/showNotifications", true).toBool());
    notificationDurationSpin->setValue(settings.value("notifications/duration", 5).toInt());
}

void SettingsTab::onStartWithWindowsChanged(bool checked)
{
    QSettings settings;
    settings.setValue("general/startWithWindows", checked);
    
    // Windows başlangıç kaydını güncelle
    // Bu kısım ileride uygulanacak
}

void SettingsTab::onMinimizeToTrayChanged(bool checked)
{
    QSettings settings;
    settings.setValue("general/minimizeToTray", checked);
}

void SettingsTab::onCheckForUpdatesChanged(bool checked)
{
    QSettings settings;
    settings.setValue("general/checkForUpdates", checked);
}

void SettingsTab::onLanguageChanged(int index)
{
    QString language = languageCombo->itemData(index).toString();
    
    QSettings settings;
    settings.setValue("appearance/language", language);
    
    // Dil değişikliği için yeniden başlatma gerektiğini bildir
    QMessageBox::information(this, tr("Dil Değişikliği"),
                            tr("Dil değişikliğinin uygulanması için uygulamayı yeniden başlatmanız gerekiyor."));
}

void SettingsTab::onThemeChanged(int index)
{
    QString theme = themeCombo->itemData(index).toString();
    
    QSettings settings;
    settings.setValue("appearance/theme", theme);
    
    // Temayı uygula
    // Bu kısım ileride uygulanacak
}

void SettingsTab::onShowNotificationsChanged(bool checked)
{
    QSettings settings;
    settings.setValue("notifications/showNotifications", checked);
    
    // Bildirim süresini etkinleştir/devre dışı bırak
    notificationDurationSpin->setEnabled(checked);
}

void SettingsTab::onNotificationDurationChanged(int value)
{
    QSettings settings;
    settings.setValue("notifications/duration", value);
}

void SettingsTab::onSaveSettingsClicked()
{
    // Tüm ayarları kaydet
    QSettings settings;
    settings.sync();
    
    QMessageBox::information(this, tr("Ayarlar Kaydedildi"),
                            tr("Ayarlar başarıyla kaydedildi."));
}

void SettingsTab::onResetSettingsClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Ayarları Sıfırla"),
                                 tr("Tüm ayarları varsayılan değerlere sıfırlamak istediğinizden emin misiniz?"),
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Ayarları sıfırla
        QSettings settings;
        settings.clear();
        
        // Varsayılan ayarları yükle
        loadSettings();
        
        QMessageBox::information(this, tr("Ayarlar Sıfırlandı"),
                                tr("Tüm ayarlar varsayılan değerlere sıfırlandı."));
    }
}

void SettingsTab::applyLanguage(const QString &language)
{
    // Mevcut çevirmeni kaldır
    if (translator) {
        qApp->removeTranslator(translator);
        delete translator;
        translator = nullptr;
    }
    
    // Yeni çevirmen oluştur
    translator = new QTranslator(qApp);
    
    // Çeviri dosyasını yükle
    if (language != "en") {
        QString translationFile = QString(":/translations/imperiumhub_%1.qm").arg(language);
        if (translator->load(translationFile)) {
            qApp->installTranslator(translator);
        }
    }
    
    // Arayüzü yeniden çevir
    emit languageChanged();
}