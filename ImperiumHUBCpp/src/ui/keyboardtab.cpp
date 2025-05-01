#include "keyboardtab.h"
#include "keyboardview.h"
#include "macroeditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QInputDialog>

KeyboardTab::KeyboardTab(DeviceManager *deviceManager, ProfileManager *profileManager, QWidget *parent)
    : QWidget(parent),
      deviceManager(deviceManager),
      profileManager(profileManager),
      keyboardController(nullptr),
      selectedKeyCode(-1),
      isConnected(false)
{
    setupUi();
    
    // Başlangıçta devre dışı bırak
    setEnabled(false);
}

void KeyboardTab::setupUi()
{
    // Ana düzen
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Profil seçimi
    QHBoxLayout *profileLayout = new QHBoxLayout();
    
    profileLayout->addWidget(new QLabel(tr("Profil:"), this));
    
    profileCombo = new QComboBox(this);
    profileLayout->addWidget(profileCombo, 1);
    
    saveProfileButton = new QPushButton(tr("Profili Kaydet"), this);
    profileLayout->addWidget(saveProfileButton);
    
    resetButton = new QPushButton(tr("Varsayılana Sıfırla"), this);
    profileLayout->addWidget(resetButton);
    
    mainLayout->addLayout(profileLayout);
    
    // Sekmeler
    tabWidget = new QTabWidget(this);
    mainLayout->addWidget(tabWidget);
    
    // Sekmeleri oluştur
    setupRgbTab();
    setupKeysTab();
    setupPerformanceTab();
    
    // Bağlantıları kur
    connect(profileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &KeyboardTab::onProfileChanged);
    connect(saveProfileButton, &QPushButton::clicked,
            this, &KeyboardTab::onSaveProfileClicked);
    connect(resetButton, &QPushButton::clicked,
            this, &KeyboardTab::onResetToDefaultsClicked);
    
    // Makro editörü
    macroEditor = new MacroEditor(profileManager, this);
    macroEditor->setWindowModality(Qt::ApplicationModal);
}

void KeyboardTab::setupRgbTab()
{
    rgbTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(rgbTab);
    
    // RGB modu
    QGroupBox *rgbModeGroup = new QGroupBox(tr("RGB Modu"), rgbTab);
    QHBoxLayout *rgbModeLayout = new QHBoxLayout(rgbModeGroup);
    
    rgbModeLayout->addWidget(new QLabel(tr("Mod:"), rgbTab));
    rgbModeCombo = new QComboBox(rgbTab);
    rgbModeCombo->addItem(tr("Statik"));
    rgbModeCombo->addItem(tr("Nefes Alma"));
    rgbModeCombo->addItem(tr("Dalga"));
    rgbModeCombo->addItem(tr("Dalgalanma"));
    rgbModeCombo->addItem(tr("Yağmur"));
    rgbModeCombo->addItem(tr("Reaktif"));
    rgbModeCombo->addItem(tr("Spektrum"));
    rgbModeCombo->addItem(tr("Yıldız"));
    rgbModeCombo->addItem(tr("Özel"));
    rgbModeCombo->addItem(tr("Kapalı"));
    rgbModeLayout->addWidget(rgbModeCombo);
    
    layout->addWidget(rgbModeGroup);
    
    // RGB rengi
    QGroupBox *rgbColorGroup = new QGroupBox(tr("RGB Rengi"), rgbTab);
    QHBoxLayout *rgbColorLayout = new QHBoxLayout(rgbColorGroup);
    
    rgbColorLayout->addWidget(new QLabel(tr("Renk:"), rgbTab));
    rgbColorButton = new QPushButton(rgbTab);
    rgbColorButton->setAutoFillBackground(true);
    rgbColorButton->setFlat(true);
    rgbColorButton->setStyleSheet("background-color: white;");
    currentRgbColor = Qt::white;
    rgbColorLayout->addWidget(rgbColorButton);
    
    layout->addWidget(rgbColorGroup);
    
    // RGB parlaklık
    QGroupBox *brightnessGroup = new QGroupBox(tr("RGB Parlaklığı"), rgbTab);
    QHBoxLayout *brightnessLayout = new QHBoxLayout(brightnessGroup);
    
    rgbBrightnessSlider = new QSlider(Qt::Horizontal, rgbTab);
    rgbBrightnessSlider->setRange(0, 100);
    rgbBrightnessSlider->setValue(100);
    brightnessLayout->addWidget(rgbBrightnessSlider);
    
    rgbBrightnessLabel = new QLabel("100%", rgbTab);
    brightnessLayout->addWidget(rgbBrightnessLabel);
    
    layout->addWidget(brightnessGroup);
    
    // RGB hızı
    QGroupBox *speedGroup = new QGroupBox(tr("RGB Efekt Hızı"), rgbTab);
    QHBoxLayout *speedLayout = new QHBoxLayout(speedGroup);
    
    rgbSpeedSlider = new QSlider(Qt::Horizontal, rgbTab);
    rgbSpeedSlider->setRange(0, 100);
    rgbSpeedSlider->setValue(50);
    speedLayout->addWidget(rgbSpeedSlider);
    
    rgbSpeedLabel = new QLabel("50%", rgbTab);
    speedLayout->addWidget(rgbSpeedLabel);
    
    layout->addWidget(speedGroup);
    
    // RGB yönü
    QGroupBox *directionGroup = new QGroupBox(tr("RGB Efekt Yönü"), rgbTab);
    QHBoxLayout *directionLayout = new QHBoxLayout(directionGroup);
    
    directionLayout->addWidget(new QLabel(tr("Yön:"), rgbTab));
    rgbDirectionCombo = new QComboBox(rgbTab);
    rgbDirectionCombo->addItem(tr("Soldan Sağa"));
    rgbDirectionCombo->addItem(tr("Sağdan Sola"));
    rgbDirectionCombo->addItem(tr("Yukarıdan Aşağıya"));
    rgbDirectionCombo->addItem(tr("Aşağıdan Yukarıya"));
    rgbDirectionCombo->addItem(tr("Dıştan İçe"));
    rgbDirectionCombo->addItem(tr("İçten Dışa"));
    directionLayout->addWidget(rgbDirectionCombo);
    
    layout->addWidget(directionGroup);
    layout->addStretch();
    
    tabWidget->addTab(rgbTab, tr("RGB Aydınlatma"));
    
    // Bağlantıları kur
    connect(rgbModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &KeyboardTab::onRgbModeChanged);
    connect(rgbBrightnessSlider, &QSlider::valueChanged,
            this, &KeyboardTab::onRgbBrightnessChanged);
    connect(rgbSpeedSlider, &QSlider::valueChanged,
            this, &KeyboardTab::onRgbSpeedChanged);
    connect(rgbDirectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &KeyboardTab::onRgbDirectionChanged);
    connect(rgbColorButton, &QPushButton::clicked,
            this, &KeyboardTab::onRgbColorClicked);
}

void KeyboardTab::setupKeysTab()
{
    keysTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(keysTab);
    
    // Klavye görünümü
    keyboardView = new KeyboardView(keysTab);
    keyboardView->setMinimumHeight(300);
    layout->addWidget(keyboardView);
    
    // Tuş fonksiyonu
    QGroupBox *keyGroup = new QGroupBox(tr("Tuş Ataması"), keysTab);
    QGridLayout *keyLayout = new QGridLayout(keyGroup);
    
    keyLayout->addWidget(new QLabel(tr("Fonksiyon:"), keysTab), 0, 0);
    keyFunctionCombo = new QComboBox(keysTab);
    keyFunctionCombo->addItem(tr("Varsayılan"));
    keyFunctionCombo->addItem(tr("Devre Dışı"));
    keyFunctionCombo->addItem(tr("Klavye Tuşu"));
    keyFunctionCombo->addItem(tr("Fare Tuşu"));
    keyFunctionCombo->addItem(tr("Medya Kontrolü"));
    keyFunctionCombo->addItem(tr("Makro"));
    keyFunctionCombo->addItem(tr("Sistem Komutu"));
    keyLayout->addWidget(keyFunctionCombo, 0, 1);
    
    keyLayout->addWidget(new QLabel(tr("Eylem:"), keysTab), 1, 0);
    keyActionCombo = new QComboBox(keysTab);
    keyLayout->addWidget(keyActionCombo, 1, 1);
    
    keyLayout->addWidget(new QLabel(tr("Makro:"), keysTab), 2, 0);
    macroCombo = new QComboBox(keysTab);
    keyLayout->addWidget(macroCombo, 2, 1);
    
    editMacroButton = new QPushButton(tr("Makro Düzenle"), keysTab);
    keyLayout->addWidget(editMacroButton, 3, 0, 1, 2);
    
    layout->addWidget(keyGroup);
    layout->addStretch();
    
    tabWidget->addTab(keysTab, tr("Tuş Atamaları"));
    
    // Bağlantıları kur
    connect(keyFunctionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &KeyboardTab::onKeyFunctionChanged);
    connect(keyActionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &KeyboardTab::onKeyActionChanged);
    connect(macroCombo, &QComboBox::currentTextChanged,
            this, &KeyboardTab::onMacroSelected);
    connect(editMacroButton, &QPushButton::clicked,
            this, &KeyboardTab::onEditMacroClicked);
    connect(keyboardView, &KeyboardView::keySelected,
            this, &KeyboardTab::onKeySelected);
}

void KeyboardTab::setupPerformanceTab()
{
    performanceTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(performanceTab);
    
    // Polling rate
    QGroupBox *pollingGroup = new QGroupBox(tr("Polling Rate"), performanceTab);
    QHBoxLayout *pollingLayout = new QHBoxLayout(pollingGroup);
    
    pollingLayout->addWidget(new QLabel(tr("Hız:"), performanceTab));
    pollingRateCombo = new QComboBox(performanceTab);
    pollingRateCombo->addItem("125 Hz", static_cast<int>(PollingRate::Hz125));
    pollingRateCombo->addItem("250 Hz", static_cast<int>(PollingRate::Hz250));
    pollingRateCombo->addItem("500 Hz", static_cast<int>(PollingRate::Hz500));
    pollingRateCombo->addItem("1000 Hz", static_cast<int>(PollingRate::Hz1000));
    pollingRateCombo->setCurrentIndex(3); // 1000 Hz varsayılan
    pollingLayout->addWidget(pollingRateCombo);
    
    layout->addWidget(pollingGroup);
    
    // Debounce Time
    QGroupBox *debounceGroup = new QGroupBox(tr("Debounce Süresi"), performanceTab);
    QHBoxLayout *debounceLayout = new QHBoxLayout(debounceGroup);
    
    debounceTimeSlider = new QSlider(Qt::Horizontal, performanceTab);
    debounceTimeSlider->setRange(1, 20);
    debounceTimeSlider->setValue(5);
    debounceLayout->addWidget(debounceTimeSlider);
    
    debounceTimeLabel = new QLabel("5 ms", performanceTab);
    debounceLayout->addWidget(debounceTimeLabel);
    
    layout->addWidget(debounceGroup);
    
    // NKRO Modu
    QGroupBox *nkroGroup = new QGroupBox(tr("NKRO Modu"), performanceTab);
    QHBoxLayout *nkroLayout = new QHBoxLayout(nkroGroup);
    
    nkroLayout->addWidget(new QLabel(tr("Mod:"), performanceTab));
    nkroModeCombo = new QComboBox(performanceTab);
    nkroModeCombo->addItem(tr("6KRO (6 Tuş)"));
    nkroModeCombo->addItem(tr("NKRO (Sınırsız)"));
    nkroLayout->addWidget(nkroModeCombo);
    
    layout->addWidget(nkroGroup);
    
    // Game Mode
    gameModeCheck = new QCheckBox(tr("Oyun Modu (Windows tuşunu devre dışı bırakır)"), performanceTab);
    layout->addWidget(gameModeCheck);
    
    // Win Key Lock
    winKeyLockCheck = new QCheckBox(tr("Windows Tuşu Kilidi"), performanceTab);
    layout->addWidget(winKeyLockCheck);
    
    layout->addStretch();
    
    tabWidget->addTab(performanceTab, tr("Performans"));
    
    // Bağlantıları kur
    connect(pollingRateCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &KeyboardTab::onPollingRateChanged);
    connect(debounceTimeSlider, &QSlider::valueChanged,
            this, &KeyboardTab::onDebounceTimeChanged);
    connect(nkroModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &KeyboardTab::onNkroModeChanged);
    connect(gameModeCheck, &QCheckBox::toggled,
            this, &KeyboardTab::onGameModeChanged);
    connect(winKeyLockCheck, &QCheckBox::toggled,
            this, &KeyboardTab::onWinKeyLockChanged);
}

void KeyboardTab::onDeviceConnected(const ConnectedDevice& device)
{
    if (device.type == DeviceType::Keyboard) {
        // Klavye kontrolcüsünü oluştur
        keyboardController = new KeyboardController(device.device, this);
        
        // Profilleri yükle
        profileCombo->clear();
        QStringList profiles = profileManager->getProfileNames(ProfileType::Keyboard);
        profileCombo->addItems(profiles);
        
        // Aktif profili seç
        QString activeProfile = profileManager->getActiveProfile(ProfileType::Keyboard);
        int index = profileCombo->findText(activeProfile);
        if (index >= 0) {
            profileCombo->setCurrentIndex(index);
        }
        
        // Arayüzü etkinleştir
        setEnabled(true);
        isConnected = true;
        
        // Arayüzü güncelle
        updateUiFromProfile();
    }
}

void KeyboardTab::onDeviceDisconnected(const ConnectedDevice& device)
{
    if (device.type == DeviceType::Keyboard) {
        // Klavye kontrolcüsünü temizle
        if (keyboardController) {
            delete keyboardController;
            keyboardController = nullptr;
        }
        
        // Sekmeyi devre dışı bırak
        setEnabled(false);
        isConnected = false;
    }
}

void KeyboardTab::onProfileChanged(int index)
{
    if (index >= 0) {
        currentProfile = profileCombo->itemText(index);
        profileManager->setActiveProfile(ProfileType::Keyboard, currentProfile);
        updateUiFromProfile();
    }
}

void KeyboardTab::onRgbModeChanged(int index)
{
    if (!isConnected || index < 0) return;
    
    RgbMode mode = static_cast<RgbMode>(rgbModeCombo->itemData(index).toInt());
    
    // Profili güncelle
    profileManager->setKeyboardRgbMode(currentProfile, mode);
    
    // Cihazı güncelle
    if (keyboardController) {
        keyboardController->setRgbMode(mode);
    }
    
    // Renk seçiciyi güncelle
    rgbColorButton->setEnabled(mode == RGB_STATIC || mode == RGB_BREATHING);
    
    // Hız ayarını güncelle
    rgbSpeedSlider->setEnabled(mode != RGB_STATIC && mode != RGB_OFF);
}

void KeyboardTab::onRgbColorClicked()
{
    if (!isConnected) return;
    
    QColorDialog colorDialog(currentRgbColor, this);
    if (colorDialog.exec() == QDialog::Accepted) {
        currentRgbColor = colorDialog.selectedColor();
        updateColorButton();
        
        // Profili güncelle
        profileManager->setKeyboardRgbColor(currentProfile, currentRgbColor);
        
        // Cihazı güncelle
        if (keyboardController) {
            keyboardController->setRgbColor(currentRgbColor);
        }
    }
}

void KeyboardTab::onRgbBrightnessChanged(int value)
{
    if (!isConnected) return;
    
    rgbBrightnessLabel->setText(tr("%1%").arg(value));
    
    // Profili güncelle
    profileManager->setKeyboardRgbBrightness(currentProfile, value);
    
    // Cihazı güncelle
    if (keyboardController) {
        keyboardController->setRgbBrightness(value);
    }
}

void KeyboardTab::onRgbSpeedChanged(int value)
{
    if (!isConnected) return;
    
    rgbSpeedLabel->setText(tr("%1%").arg(value));
    
    // Profili güncelle
    profileManager->setKeyboardRgbSpeed(currentProfile, value);
    
    // Cihazı güncelle
    if (keyboardController) {
        keyboardController->setRgbSpeed(value);
    }
}

void KeyboardTab::onActuationPointChanged(double value)
{
    if (!isConnected) return;
    
    // Profili güncelle
    profileManager->setKeyboardActuationPoint(currentProfile, value);
    
    // Cihazı güncelle
    if (keyboardController) {
        keyboardController->setActuationPoint(value);
    }
}

void KeyboardTab::onRapidTriggerChanged(bool checked)
{
    if (!isConnected) return;
    
    // Profili güncelle
    profileManager->setKeyboardRapidTrigger(currentProfile, checked);
    
    // Cihazı güncelle
    if (keyboardController) {
        keyboardController->setRapidTrigger(checked);
    }
}

void KeyboardTab::onDynamicKeystrokeChanged(bool checked)
{
    if (!isConnected) return;
    
    // Profili güncelle
    profileManager->setKeyboardDynamicKeystroke(currentProfile, checked);
    
    // Cihazı güncelle
    if (keyboardController) {
        keyboardController->setDynamicKeystroke(checked);
    }
}

void KeyboardTab::onKeySelected(int keyIndex)
{
    if (!isConnected) return;
    
    selectedKeyIndex = keyIndex;
    
    // Tuş ayarlarını güncelle
    QJsonObject keyMapping = profileManager->getKeyboardKeyMapping(currentProfile, keyIndex);
    
    int function = keyMapping["function"].toInt(0);
    int keyCode = keyMapping["keyCode"].toInt(0);
    QString macroName = keyMapping["macroName"].toString("");
    
    // Fonksiyon seçimini güncelle
    int functionIndex = keyFunctionCombo->findData(function);
    if (functionIndex >= 0) {
        keyFunctionCombo->setCurrentIndex(functionIndex);
    }
    
    // Tuş kodu seçimini güncelle
    int keyCodeIndex = keyCodeCombo->findData(keyCode);
    if (keyCodeIndex >= 0) {
        keyCodeCombo->setCurrentIndex(keyCodeIndex);
    }
    
    // Makro seçimini güncelle
    int macroIndex = macroCombo->findText(macroName);
    if (macroIndex >= 0) {
        macroCombo->setCurrentIndex(macroIndex);
    }
    
    // Arayüzü güncelle
    onKeyFunctionChanged(keyFunctionCombo->currentIndex());
}

void KeyboardTab::onKeyFunctionChanged(int index)
{
    if (!isConnected || index < 0 || selectedKeyIndex < 0) return;
    
    KeyFunction function = static_cast<KeyFunction>(keyFunctionCombo->itemData(index).toInt());
    
    // Arayüzü güncelle
    keyCodeCombo->setEnabled(function != KEY_DEFAULT && function != KEY_DISABLED && function != KEY_MACRO);
    macroCombo->setEnabled(function == KEY_MACRO);
    
    // Profili güncelle
    int keyCode = keyCodeCombo->currentData().toInt();
    QString macroName = macroCombo->currentText();
    
    profileManager->setKeyboardKeyMapping(currentProfile, selectedKeyIndex, function, keyCode, macroName);
    
    // Cihazı güncelle
    if (keyboardController) {
        KeyConfig config;
        config.function = function;
        config.keyCode = keyCode;
        config.macroName = macroName;
        
        keyboardController->setKeyFunction(selectedKeyIndex, config);
    }
}

void KeyboardTab::onSaveProfileClicked()
{
    if (!isConnected) return;
    
    // Profil adını iste
    bool ok;
    QString profileName = QInputDialog::getText(this, tr("Profil Kaydet"),
                                               tr("Profil adı:"), QLineEdit::Normal,
                                               currentProfile, &ok);
    if (ok && !profileName.isEmpty()) {
        // Profili kaydet
        if (profileManager->createProfile(ProfileType::Keyboard, profileName)) {
            // Profil listesini güncelle
            profileCombo->clear();
            profileCombo->addItems(profileManager->getProfileNames(ProfileType::Keyboard));
            
            // Yeni profili seç
            int index = profileCombo->findText(profileName);
            if (index >= 0) {
                profileCombo->setCurrentIndex(index);
            }
            
            QMessageBox::information(this, tr("Profil Kaydedildi"),
                                    tr("Profil başarıyla kaydedildi: %1").arg(profileName));
        } else {
            QMessageBox::warning(this, tr("Hata"),
                                tr("Profil kaydedilemedi: %1").arg(profileName));
        }
    }
}

void KeyboardTab::onResetToDefaultsClicked()
{
    if (!isConnected) return;
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Varsayılana Sıfırla"),
                                 tr("Tüm ayarları varsayılan değerlere sıfırlamak istediğinizden emin misiniz?"),
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Cihazı sıfırla
        if (keyboardController) {
            keyboardController->resetToDefaults();
        }
        
        // Arayüzü güncelle
        updateUiFromProfile();
    }
}

void KeyboardTab::updateUiFromProfile()
{
    if (!isConnected) return;
    
    // RGB ayarları
    int rgbMode = profileManager->getKeyboardRgbMode(currentProfile);
    int modeIndex = rgbModeCombo->findData(rgbMode);
    if (modeIndex >= 0) {
        rgbModeCombo->setCurrentIndex(modeIndex);
    }
    
    currentRgbColor = profileManager->getKeyboardRgbColor(currentProfile);
    updateColorButton();
    
    int brightness = profileManager->getKeyboardRgbBrightness(currentProfile);
    rgbBrightnessSlider->setValue(brightness);
    rgbBrightnessLabel->setText(tr("%1%").arg(brightness));
    
    int speed = profileManager->getKeyboardRgbSpeed(currentProfile);
    rgbSpeedSlider->setValue(speed);
    rgbSpeedLabel->setText(tr("%1%").arg(speed));
    
    // Hall Effect ayarları
    double actuationPoint = profileManager->getKeyboardActuationPoint(currentProfile);
    actuationPointSpin->setValue(actuationPoint);
    
    bool rapidTrigger = profileManager->getKeyboardRapidTrigger(currentProfile);
    rapidTriggerCheck->setChecked(rapidTrigger);
    
    bool dynamicKeystroke = profileManager->getKeyboardDynamicKeystroke(currentProfile);
    dynamicKeystrokeCheck->setChecked(dynamicKeystroke);
    
    // Klavye görünümünü güncelle
    updateKeyboardView();
}

void KeyboardTab::updateKeyboardView()
{
    // Bu fonksiyon, klavye düzenini görsel olarak gösterecek
    // Şimdilik boş bırakıyoruz, ileride uygulanacak
}

void KeyboardTab::updateColorButton()
{
    // Renk butonunun arka plan rengini güncelle
    rgbColorButton->setStyleSheet(QString("background-color: %1;").arg(currentRgbColor.name()));
}

void KeyboardTab::onRgbDirectionChanged(int index)
{
    if (!isConnected) return;
    
    // Profili güncelle
    profileManager->setKeyboardRgbDirection(currentProfile, index);
    
    // Cihazı güncelle
    if (keyboardController) {
        keyboardController->setRgbDirection(index);
    }
}

void KeyboardTab::onPollingRateChanged(int index)
{
    if (!isConnected) return;
    
    PollingRate rate = static_cast<PollingRate>(pollingRateCombo->itemData(index).toInt());
    
    // Profili güncelle
    profileManager->setKeyboardPollingRate(currentProfile, static_cast<int>(rate));
    
    // Cihazı güncelle
    if (keyboardController) {
        keyboardController->setPollingRate(rate);
    }
}

void KeyboardTab::onDebounceTimeChanged(int value)
{
    if (!isConnected) return;
    
    debounceTimeLabel->setText(tr("%1 ms").arg(value));
    
    // Profili güncelle
    profileManager->setKeyboardDebounceTime(currentProfile, value);
    
    // Cihazı güncelle
    if (keyboardController) {
        keyboardController->setDebounceTime(value);
    }
}

void KeyboardTab::onNkroModeChanged(int index)
{
    if (!isConnected) return;
    
    // Profili güncelle
    profileManager->setKeyboardNkroMode(currentProfile, index);
    
    // Cihazı güncelle
    if (keyboardController) {
        keyboardController->setNkroMode(index);
    }
}

void KeyboardTab::onGameModeChanged(bool checked)
{
    if (!isConnected) return;
    
    // Profili güncelle
    profileManager->setKeyboardGameMode(currentProfile, checked);
    
    // Cihazı güncelle
    if (keyboardController) {
        keyboardController->setGameMode(checked);
    }
}

void KeyboardTab::onWinKeyLockChanged(bool checked)
{
    if (!isConnected) return;
    
    // Profili güncelle
    profileManager->setKeyboardWinKeyLock(currentProfile, checked);
    
    // Cihazı güncelle
    if (keyboardController) {
        keyboardController->setWinKeyLock(checked);
    }
}

void KeyboardTab::onKeyActionChanged(int index)
{
    if (!isConnected || index < 0 || selectedKeyIndex < 0) return;
    
    // Profili güncelle
    KeyFunction function = static_cast<KeyFunction>(keyFunctionCombo->currentData().toInt());
    int keyCode = keyActionCombo->currentData().toInt();
    QString macroName = macroCombo->currentText();
    
    profileManager->setKeyboardKeyMapping(currentProfile, selectedKeyIndex, function, keyCode, macroName);
    
    // Cihazı güncelle
    if (keyboardController) {
        KeyConfig config;
        config.function = function;
        config.keyCode = keyCode;
        config.macroName = macroName;
        
        keyboardController->setKeyFunction(selectedKeyIndex, config);
    }
}

void KeyboardTab::onMacroSelected(const QString& macroName)
{
    if (!isConnected || selectedKeyIndex < 0) return;
    
    // Profili güncelle
    KeyFunction function = static_cast<KeyFunction>(keyFunctionCombo->currentData().toInt());
    int keyCode = keyActionCombo->currentData().toInt();
    
    profileManager->setKeyboardKeyMapping(currentProfile, selectedKeyIndex, function, keyCode, macroName);
    
    // Cihazı güncelle
    if (keyboardController) {
        KeyConfig config;
        config.function = function;
        config.keyCode = keyCode;
        config.macroName = macroName;
        
        keyboardController->setKeyFunction(selectedKeyIndex, config);
    }
}

void KeyboardTab::onEditMacroClicked()
{
    if (!isConnected) return;
    
    // Makro editörünü göster
    macroEditor->setCurrentMacro(macroCombo->currentText());
    macroEditor->show();
    
    // Makro editörü kapandığında makro listesini güncelle
    connect(macroEditor, &QDialog::finished, this, [this]() {
        // Makro listesini güncelle
        QString currentMacro = macroCombo->currentText();
        macroCombo->clear();
        macroCombo->addItems(profileManager->getMacroNames());
        
        // Önceki seçimi geri yükle
        int index = macroCombo->findText(currentMacro);
        if (index >= 0) {
            macroCombo->setCurrentIndex(index);
        }
    });
}