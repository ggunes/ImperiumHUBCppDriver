#include "mousetab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QInputDialog>

MouseTab::MouseTab(DeviceManager *deviceManager, ProfileManager *profileManager, QWidget *parent)
    : QWidget(parent),
      deviceManager(deviceManager),
      profileManager(profileManager),
      mouseController(nullptr),
      selectedButtonIndex(-1),
      isConnected(false)
{
    setupUi();
    
    // Başlangıçta devre dışı bırak
    setEnabled(false);
}

void MouseTab::setupUi()
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
    setupDpiTab();
    setupRgbTab();
    setupPerformanceTab();
    setupButtonsTab();
    
    // Bağlantıları kur
    connect(profileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MouseTab::onProfileChanged);
    connect(saveProfileButton, &QPushButton::clicked,
            this, &MouseTab::onSaveProfileClicked);
    connect(resetButton, &QPushButton::clicked,
            this, &MouseTab::onResetToDefaultsClicked);
}

void MouseTab::setupDpiTab()
{
    dpiTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(dpiTab);
    
    // DPI seviyesi seçimi
    QGroupBox *dpiLevelGroup = new QGroupBox(tr("DPI Seviyesi"), dpiTab);
    QGridLayout *dpiLevelLayout = new QGridLayout(dpiLevelGroup);
    
    dpiLevelLayout->addWidget(new QLabel(tr("Seviye:"), dpiTab), 0, 0);
    dpiLevelCombo = new QComboBox(dpiTab);
    for (int i = 1; i <= 5; i++) {
        dpiLevelCombo->addItem(tr("Seviye %1").arg(i));
    }
    dpiLevelLayout->addWidget(dpiLevelCombo, 0, 1);
    
    dpiLevelLayout->addWidget(new QLabel(tr("DPI Değeri:"), dpiTab), 1, 0);
    dpiValueSpin = new QSpinBox(dpiTab);
    dpiValueSpin->setRange(50, 26000);
    dpiValueSpin->setSingleStep(50);
    dpiValueSpin->setValue(800);
    dpiLevelLayout->addWidget(dpiValueSpin, 1, 1);
    
    dpiLevelLayout->addWidget(new QLabel(tr("Renk:"), dpiTab), 2, 0);
    dpiColorButton = new QPushButton(dpiTab);
    dpiColorButton->setAutoFillBackground(true);
    dpiColorButton->setFlat(true);
    dpiColorButton->setStyleSheet("background-color: red;");
    currentDpiColor = Qt::red;
    dpiLevelLayout->addWidget(dpiColorButton, 2, 1);
    
    dpiEnabledCheck = new QCheckBox(tr("Bu seviyeyi etkinleştir"), dpiTab);
    dpiEnabledCheck->setChecked(true);
    dpiLevelLayout->addWidget(dpiEnabledCheck, 3, 0, 1, 2);
    
    layout->addWidget(dpiLevelGroup);
    
    // Polling rate
    QGroupBox *pollingGroup = new QGroupBox(tr("Polling Rate"), dpiTab);
    QHBoxLayout *pollingLayout = new QHBoxLayout(pollingGroup);
    
    pollingLayout->addWidget(new QLabel(tr("Hız:"), dpiTab));
    pollingRateCombo = new QComboBox(dpiTab);
    pollingRateCombo->addItem("125 Hz", static_cast<int>(PollingRate::Hz125));
    pollingRateCombo->addItem("250 Hz", static_cast<int>(PollingRate::Hz250));
    pollingRateCombo->addItem("500 Hz", static_cast<int>(PollingRate::Hz500));
    pollingRateCombo->addItem("1000 Hz", static_cast<int>(PollingRate::Hz1000));
    pollingRateCombo->setCurrentIndex(3); // 1000 Hz varsayılan
    pollingLayout->addWidget(pollingRateCombo);
    
    layout->addWidget(pollingGroup);
    layout->addStretch();
    
    tabWidget->addTab(dpiTab, tr("DPI Ayarları"));
    
    // Bağlantıları kur
    connect(dpiLevelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MouseTab::onDpiLevelChanged);
    connect(dpiValueSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MouseTab::onDpiValueChanged);
    connect(dpiColorButton, &QPushButton::clicked,
            this, &MouseTab::onDpiColorClicked);
    connect(dpiEnabledCheck, &QCheckBox::toggled,
            this, &MouseTab::onDpiEnabledChanged);
    connect(pollingRateCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MouseTab::onPollingRateChanged);
}

void MouseTab::setupRgbTab()
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
    rgbModeCombo->addItem(tr("Reaktif"));
    rgbModeCombo->addItem(tr("Dalgalanma"));
    rgbModeCombo->addItem(tr("Yağmur"));
    rgbModeCombo->addItem(tr("Spektrum"));
    rgbModeCombo->addItem(tr("Özel"));
    rgbModeCombo->addItem(tr("Kapalı"));
    rgbModeLayout->addWidget(rgbModeCombo);
    
    layout->addWidget(rgbModeGroup);
    
    // RGB renkleri
    QGroupBox *rgbColorGroup = new QGroupBox(tr("RGB Renkleri"), rgbTab);
    QGridLayout *rgbColorLayout = new QGridLayout(rgbColorGroup);
    
    rgbColorLayout->addWidget(new QLabel(tr("Logo Rengi:"), rgbTab), 0, 0);
    logoColorButton = new QPushButton(rgbTab);
    logoColorButton->setAutoFillBackground(true);
    logoColorButton->setFlat(true);
    logoColorButton->setStyleSheet("background-color: white;");
    currentLogoColor = Qt::white;
    rgbColorLayout->addWidget(logoColorButton, 0, 1);
    
    rgbColorLayout->addWidget(new QLabel(tr("Kaydırma Tekerleği Rengi:"), rgbTab), 1, 0);
    scrollColorButton = new QPushButton(rgbTab);
    scrollColorButton->setAutoFillBackground(true);
    scrollColorButton->setFlat(true);
    scrollColorButton->setStyleSheet("background-color: white;");
    currentScrollColor = Qt::white;
    rgbColorLayout->addWidget(scrollColorButton, 1, 1);
    
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
    layout->addStretch();
    
    tabWidget->addTab(rgbTab, tr("RGB Aydınlatma"));
    
    // Bağlantıları kur
    connect(rgbModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MouseTab::onRgbModeChanged);
    connect(logoColorButton, &QPushButton::clicked,
            this, &MouseTab::onLogoColorClicked);
    connect(scrollColorButton, &QPushButton::clicked,
            this, &MouseTab::onScrollColorClicked);
    connect(rgbBrightnessSlider, &QSlider::valueChanged,
            this, &MouseTab::onRgbBrightnessChanged);
    connect(rgbSpeedSlider, &QSlider::valueChanged,
            this, &MouseTab::onRgbSpeedChanged);
}

void MouseTab::setupPerformanceTab()
{
    performanceTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(performanceTab);
    
    // Lift-Off Distance
    QGroupBox *lodGroup = new QGroupBox(tr("Lift-Off Distance"), performanceTab);
    QHBoxLayout *lodLayout = new QHBoxLayout(lodGroup);
    
    lodSlider = new QSlider(Qt::Horizontal, performanceTab);
    lodSlider->setRange(1, 5);
    lodSlider->setValue(2);
    lodLayout->addWidget(lodSlider);
    
    lodLabel = new QLabel("2 mm", performanceTab);
    lodLayout->addWidget(lodLabel);
    
    layout->addWidget(lodGroup);
    
    // Angle Snapping
    angleSnappingCheck = new QCheckBox(tr("Angle Snapping (Düz çizgi çizmeyi kolaylaştırır)"), performanceTab);
    layout->addWidget(angleSnappingCheck);
    
    // Motion Sync
    motionSyncCheck = new QCheckBox(tr("Motion Sync (Daha tutarlı izleme)"), performanceTab);
    motionSyncCheck->setChecked(true);
    layout->addWidget(motionSyncCheck);
    
    // Debounce Time
    QGroupBox *debounceGroup = new QGroupBox(tr("Debounce Süresi"), performanceTab);
    QHBoxLayout *debounceLayout = new QHBoxLayout(debounceGroup);
    
    debounceLayout->addWidget(new QLabel(tr("Süre (ms):"), performanceTab));
    debounceTimeSpin = new QDoubleSpinBox(performanceTab);
    debounceTimeSpin->setRange(0.2, 16.0);
    debounceTimeSpin->setSingleStep(0.1);
    debounceTimeSpin->setValue(4.0);
    debounceLayout->addWidget(debounceTimeSpin);
    
    layout->addWidget(debounceGroup);
    
    // Kalibrasyon
    calibrateButton = new QPushButton(tr("Yüzeyi Kalibre Et"), performanceTab);
    layout->addWidget(calibrateButton);
    
    layout->addStretch();
    
    tabWidget->addTab(performanceTab, tr("Performans"));
    
    // Bağlantıları kur
    connect(lodSlider, &QSlider::valueChanged,
            this, &MouseTab::onLodChanged);
    connect(angleSnappingCheck, &QCheckBox::toggled,
            this, &MouseTab::onAngleSnappingChanged);
    connect(motionSyncCheck, &QCheckBox::toggled,
            this, &MouseTab::onMotionSyncChanged);
    connect(debounceTimeSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MouseTab::onDebounceTimeChanged);
    connect(calibrateButton, &QPushButton::clicked,
            this, &MouseTab::onCalibrateClicked);
}

void MouseTab::setupButtonsTab()
{
    buttonsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(buttonsTab);
    
    // Fare görünümü
    mouseView = new QWidget(buttonsTab);
    mouseView->setMinimumHeight(200);
    mouseView->setStyleSheet("background-color: #222; border-radius: 10px;");
    layout->addWidget(mouseView);
    
    // Tuş fonksiyonu
    QGroupBox *buttonGroup = new QGroupBox(tr("Tuş Ataması"), buttonsTab);
    QGridLayout *buttonLayout = new QGridLayout(buttonGroup);
    
    buttonLayout->addWidget(new QLabel(tr("Fonksiyon:"), buttonsTab), 0, 0);
    buttonFunctionCombo = new QComboBox(buttonsTab);
    buttonFunctionCombo->addItem(tr("Varsayılan"));
    buttonFunctionCombo->addItem(tr("Devre Dışı"));
    buttonFunctionCombo->addItem(tr("Klavye Tuşu"));
    buttonFunctionCombo->addItem(tr("Fare Tuşu"));
    buttonFunctionCombo->addItem(tr("Medya Kontrolü"));
    buttonFunctionCombo->addItem(tr("Makro"));
    buttonFunctionCombo->addItem(tr("Sistem Komutu"));
    buttonLayout->addWidget(buttonFunctionCombo, 0, 1);
    
    buttonLayout->addWidget(new QLabel(tr("Eylem:"), buttonsTab), 1, 0);
    buttonActionCombo = new QComboBox(buttonsTab);
    buttonLayout->addWidget(buttonActionCombo, 1, 1);
    
    buttonLayout->addWidget(new QLabel(tr("Makro:"), buttonsTab), 2, 0);
    macroCombo = new QComboBox(buttonsTab);
    buttonLayout->addWidget(macroCombo, 2, 1);
    
    layout->addWidget(buttonGroup);
    layout->addStretch();
    
    tabWidget->addTab(buttonsTab, tr("Tuş Atamaları"));
    
    // Bağlantıları kur
    connect(buttonFunctionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MouseTab::onButtonFunctionChanged);
}

void MouseTab::onDeviceConnected(const ConnectedDevice& device)
{
    if (device.type == DeviceType::Mouse) {
        // Fare kontrolcüsünü oluştur
        mouseController = new MouseController(device.device, this);
        
        // Profilleri yükle
        profileCombo->clear();
        QStringList profiles = profileManager->getProfileNames(ProfileType::Mouse);
        profileCombo->addItems(profiles);
        
        // Aktif profili seç
        QString activeProfile = profileManager->getActiveProfile(ProfileType::Mouse);
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

void MouseTab::onDeviceDisconnected(const ConnectedDevice& device)
{
    if (device.type == DeviceType::Mouse) {
        // Fare kontrolcüsünü temizle
        if (mouseController) {
            delete mouseController;
            mouseController = nullptr;
        }
        
        // Arayüzü devre dışı bırak
        setEnabled(false);
        isConnected = false;
    }
}

void MouseTab::onProfileChanged(int index)
{
    if (index >= 0 && mouseController) {
        currentProfile = profileCombo->itemText(index);
        profileManager->setActiveProfile(ProfileType::Mouse, currentProfile);
        updateUiFromProfile();
    }
}

void MouseTab::updateUiFromProfile()
{
    if (!isConnected || currentProfile.isEmpty()) {
        return;
    }
    
    // DPI ayarları
    int activeDpiLevel = profileManager->getMouseActiveDpiLevel(currentProfile);
    dpiLevelCombo->setCurrentIndex(activeDpiLevel);
    updateDpiControls();
    
    // Polling rate
    int pollingRate = profileManager->getMousePollingRate(currentProfile);
    int pollingIndex = pollingRateCombo->findData(pollingRate);
    if (pollingIndex >= 0) {
        pollingRateCombo->setCurrentIndex(pollingIndex);
    }
    
    // RGB ayarları
    int rgbMode = profileManager->getMouseRgbMode(currentProfile);
    rgbModeCombo->setCurrentIndex(rgbMode);
    
    currentLogoColor = profileManager->getMouseLogoColor(currentProfile);
    logoColorButton->setStyleSheet(QString("background-color: %1;").arg(currentLogoColor.name()));
    
    currentScrollColor = profileManager->getMouseScrollColor(currentProfile);
    scrollColorButton->setStyleSheet(QString("background-color: %1;").arg(currentScrollColor.name()));
    
    int brightness = profileManager->getMouseRgbBrightness(currentProfile);
    rgbBrightnessSlider->setValue(brightness);
    rgbBrightnessLabel->setText(QString("%1%").arg(brightness));
    
    int speed = profileManager->getMouseRgbSpeed(currentProfile);
    rgbSpeedSlider->setValue(speed);
    rgbSpeedLabel->setText(QString("%1%").arg(speed));
    
    // Performans ayarları
    int lod = profileManager->getMouseLiftOffDistance(currentProfile);
    lodSlider->setValue(lod);
    lodLabel->setText(QString("%1 mm").arg(lod));
    
    bool angleSnapping = profileManager->getMouseAngleSnapping(currentProfile);
    angleSnappingCheck->setChecked(angleSnapping);
    
    bool motionSync = profileManager->getMouseMotionSync(currentProfile);
    motionSyncCheck->setChecked(motionSync);
    
    double debounceTime = profileManager->getMouseDebounceTime(currentProfile);
    debounceTimeSpin->setValue(debounceTime);
    
    // Fare görünümünü güncelle
    updateMouseView();
}

void MouseTab::updateDpiControls()
{
    if (!isConnected || currentProfile.isEmpty()) {
        return;
    }
    
    int level = dpiLevelCombo->currentIndex();
    QJsonObject dpiLevel = profileManager->getMouseDpiLevel(currentProfile, level);
    
    int dpi = dpiLevel["dpi"].toInt();
    dpiValueSpin->setValue(dpi);
    
    QString colorStr = dpiLevel["color"].toString();
    currentDpiColor = QColor(colorStr);
    dpiColorButton->setStyleSheet(QString("background-color: %1;").arg(colorStr));
    
    bool enabled = dpiLevel["enabled"].toBool();
    dpiEnabledCheck->setChecked(enabled);
}

void MouseTab::updateMouseView()
{
    // Bu fonksiyon, fare görünümünü güncelleyecek
    // Şimdilik boş bırakıyoruz, ileride uygulanacak
}

void MouseTab::onDpiLevelChanged(int index)
{
    updateDpiControls();
}

void MouseTab::onDpiValueChanged(int value)
{
    if (!isConnected || !mouseController) {
        return;
    }
    
    int level = dpiLevelCombo->currentIndex();
    mouseController->setDpiLevel(level, value, currentDpiColor);
    
    // Profili güncelle
    profileManager->setMouseDpiLevel(currentProfile, level, value, currentDpiColor, dpiEnabledCheck->isChecked());
}

void MouseTab::onDpiColorClicked()
{
    QColor color = QColorDialog::getColor(currentDpiColor, this, tr("DPI Rengi Seç"));
    if (color.isValid()) {
        currentDpiColor = color;
        dpiColorButton->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        
        if (isConnected && mouseController) {
            int level = dpiLevelCombo->currentIndex();
            int dpi = dpiValueSpin->value();
            mouseController->setDpiLevel(level, dpi, color);
            
            // Profili güncelle
            profileManager->setMouseDpiLevel(currentProfile, level, dpi, color, dpiEnabledCheck->isChecked());
        }
    }
}

void MouseTab::onDpiEnabledChanged(bool checked)
{
    if (!isConnected || !mouseController) {
        return;
    }
    
    int level = dpiLevelCombo->currentIndex();
    int dpi = dpiValueSpin->value();
    mouseController->setDpiLevelEnabled(level, checked);
    
    // Profili güncelle
    profileManager->setMouseDpiLevel(currentProfile, level, dpi, currentDpiColor, checked);
}

void MouseTab::onPollingRateChanged(int index)
{
    if (!isConnected || !mouseController) {
        return;
    }
    
    PollingRate rate = static_cast<PollingRate>(pollingRateCombo->itemData(index).toInt());
    mouseController->setPollingRate(rate);
    
    // Profili güncelle
    profileManager->setMousePollingRate(currentProfile, static_cast<int>(rate));
}

void MouseTab::onRgbModeChanged(int index)
{
    if (!isConnected || !mouseController) {
        return;
    }
    
    mouseController->setRgbMode(index);
    
    // Profili güncelle
    profileManager->setMouseRgbMode(currentProfile, index);
}

void MouseTab::onLogoColorClicked()
{
    QColor color = QColorDialog::getColor(currentLogoColor, this, tr("Logo Rengi Seç"));
    if (color.isValid()) {
        currentLogoColor = color;
        logoColorButton->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        
        if (isConnected && mouseController) {
            mouseController->setLogoColor(color);
            
            // Profili güncelle
            profileManager->setMouseLogoColor(currentProfile, color);
        }
    }
}

void MouseTab::onScrollColorClicked()
{
    QColor color = QColorDialog::getColor(currentScrollColor, this, tr("Kaydırma Tekerleği Rengi Seç"));
    if (color.isValid()) {
        currentScrollColor = color;
        scrollColorButton->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        
        if (isConnected && mouseController) {
            mouseController->setScrollColor(color);
            
            // Profili güncelle
            profileManager->setMouseScrollColor(currentProfile, color);
        }
    }
}

void MouseTab::onRgbBrightnessChanged(int value)
{
    rgbBrightnessLabel->setText(QString("%1%").arg(value));
    
    if (!isConnected || !mouseController) {
        return;
    }
    
    mouseController->setRgbBrightness(value);
    
    // Profili güncelle
    profileManager->setMouseRgbBrightness(currentProfile, value);
}

void MouseTab::onRgbSpeedChanged(int value)
{
    rgbSpeedLabel->setText(QString("%1%").arg(value));
    
    if (!isConnected || !mouseController) {
        return;
    }
    
    mouseController->setRgbSpeed(value);
    
    // Profili güncelle
    profileManager->setMouseRgbSpeed(currentProfile, value);
}

void MouseTab::onLodChanged(int value)
{
    lodLabel->setText(QString("%1 mm").arg(value));
    
    if (!isConnected || !mouseController) {
        return;
    }
    
    mouseController->setLiftOffDistance(value);
    
    // Profili güncelle
    profileManager->setMouseLiftOffDistance(currentProfile, value);
}

void MouseTab::onAngleSnappingChanged(bool checked)
{
    if (!isConnected || !mouseController) {
        return;
    }
    
    mouseController->setAngleSnapping(checked);
    
    // Profili güncelle
    profileManager->setMouseAngleSnapping(currentProfile, checked);
}

void MouseTab::onMotionSyncChanged(bool checked)
{
    if (!isConnected || !mouseController) {
        return;
    }
    
    mouseController->setMotionSync(checked);
    
    // Profili güncelle
    profileManager->setMouseMotionSync(currentProfile, checked);
}

void MouseTab::onDebounceTimeChanged(double value)
{
    if (!isConnected || !mouseController) {
        return;
    }
    
    mouseController->setDebounceTime(value);
    
    // Profili güncelle
    profileManager->setMouseDebounceTime(currentProfile, value);
}

void MouseTab::onCalibrateClicked()
{
    if (!isConnected || !mouseController) {
        return;
    }
    
    QMessageBox::information(this, tr("Yüzey Kalibrasyonu"),
                            tr("Kalibrasyon başlatılıyor. Lütfen fareyi düz bir yüzeyde hareket ettirin."));
    
    mouseController->calibrateSurface();
}

void MouseTab::onButtonSelected(int buttonIndex)
{
    selectedButtonIndex = buttonIndex;
    
    if (!isConnected || currentProfile.isEmpty() || buttonIndex < 0) {
        return;
    }
    
    // Seçilen tuşun ayarlarını yükle
    QJsonObject buttonMapping = profileManager->getMouseButtonMapping(currentProfile, buttonIndex);
    
    int function = buttonMapping["function"].toInt();
    buttonFunctionCombo->setCurrentIndex(function);
    
    // Eylem listesini güncelle
    onButtonFunctionChanged(function);
    
    int action = buttonMapping["action"].toInt();
    int actionIndex = buttonActionCombo->findData(action);
    if (actionIndex >= 0) {
        buttonActionCombo->setCurrentIndex(actionIndex);
    }
    
    // Makro seçimi
    QString macroName = buttonMapping["macro"].toString();
    int macroIndex = macroCombo->findText(macroName);
    if (macroIndex >= 0) {
        macroCombo->setCurrentIndex(macroIndex);
    }
}

void MouseTab::onButtonFunctionChanged(int index)
{
    // Eylem listesini güncelle
    buttonActionCombo->clear();
    
    switch (index) {
        case 0: // Varsayılan
            buttonActionCombo->setEnabled(false);
            macroCombo->setEnabled(false);
            break;
            
        case 1: // Devre Dışı
            buttonActionCombo->setEnabled(false);
            macroCombo->setEnabled(false);
            break;
            
        case 2: // Klavye Tuşu
            buttonActionCombo->setEnabled(true);
            macroCombo->setEnabled(false);
            
            // Klavye tuşlarını ekle
            buttonActionCombo->addItem("A", Qt::Key_A);
            buttonActionCombo->addItem("B", Qt::Key_B);
            buttonActionCombo->addItem("C", Qt::Key_C);
            // Diğer tuşlar...
            break;
            
        case 3: // Fare Tuşu
            buttonActionCombo->setEnabled(true);
            macroCombo->setEnabled(false);
            
            // Fare tuşlarını ekle
            buttonActionCombo->addItem(tr("Sol Tık"), Qt::LeftButton);
            buttonActionCombo->addItem(tr("Sağ Tık"), Qt::RightButton);
            buttonActionCombo->addItem(tr("Orta Tık"), Qt::MiddleButton);
            buttonActionCombo->addItem(tr("İleri"), Qt::XButton1);
            buttonActionCombo->addItem(tr("Geri"), Qt::XButton2);
            break;
            
        case 4: // Medya Kontrolü
            buttonActionCombo->setEnabled(true);
            macroCombo->setEnabled(false);
            
            // Medya kontrollerini ekle
            buttonActionCombo->addItem(tr("Oynat/Duraklat"), 0x01);
            buttonActionCombo->addItem(tr("Durdur"), 0x02);
            buttonActionCombo->addItem(tr("Önceki Parça"), 0x03);
            buttonActionCombo->addItem(tr("Sonraki Parça"), 0x04);
            buttonActionCombo->addItem(tr("Sesi Aç"), 0x05);
            buttonActionCombo->addItem(tr("Sesi Kıs"), 0x06);
            buttonActionCombo->addItem(tr("Sesi Kapat"), 0x07);
            break;
            
        case 5: // Makro
            buttonActionCombo->setEnabled(false);
            macroCombo->setEnabled(true);
            
            // Makro listesini güncelle
            macroCombo->clear();
            // Makroları profil yöneticisinden al
            // Bu kısım ileride uygulanacak
            break;
            
        case 6: // Sistem Komutu
            buttonActionCombo->setEnabled(true);
            macroCombo->setEnabled(false);
            
            // Sistem komutlarını ekle
            buttonActionCombo->addItem(tr("DPI Yukarı"), 0x01);
            buttonActionCombo->addItem(tr("DPI Aşağı"), 0x02);
            buttonActionCombo->addItem(tr("DPI Döngüsü"), 0x03);
            buttonActionCombo->addItem(tr("Profil Değiştir"), 0x04);
            buttonActionCombo->addItem(tr("Sniper Modu"), 0x05);
            break;
    }
    
    // Profili güncelle
    if (isConnected && !currentProfile.isEmpty() && selectedButtonIndex >= 0) {
        int function = index;
        int action = 0;
        if (buttonActionCombo->isEnabled() && buttonActionCombo->count() > 0) {
            action = buttonActionCombo->itemData(buttonActionCombo->currentIndex()).toInt();
        }
        
        QString macroName;
        if (macroCombo->isEnabled() && macroCombo->count() > 0) {
            macroName = macroCombo->currentText();
        }
        
        profileManager->setMouseButtonMapping(currentProfile, selectedButtonIndex, function, action, macroName);
        
        // Fare kontrolcüsünü güncelle
        if (mouseController) {
            mouseController->setButtonFunction(selectedButtonIndex, function, action, macroName);
        }
    }
}

void MouseTab::onSaveProfileClicked()
{
    if (currentProfile.isEmpty()) {
        // Yeni profil oluştur
        bool ok;
        QString name = QInputDialog::getText(this, tr("Profil Kaydet"),
                                            tr("Profil adı:"), QLineEdit::Normal,
                                            tr("Yeni Profil"), &ok);
        if (ok && !name.isEmpty()) {
            if (profileManager->createProfile(ProfileType::Mouse, name)) {
                currentProfile = name;
                profileCombo->addItem(name);
                profileCombo->setCurrentText(name);
                
                // Aktif profili güncelle
                profileManager->setActiveProfile(ProfileType::Mouse, name);
                
                // Profili kaydet
                profileManager->saveProfiles();
                
                QMessageBox::information(this, tr("Profil Kaydedildi"),
                                        tr("Profil başarıyla kaydedildi: %1").arg(name));
            } else {
                QMessageBox::warning(this, tr("Hata"),
                                    tr("Profil kaydedilemedi: %1").arg(name));
            }
        }
    } else {
        // Mevcut profili güncelle
        if (profileManager->saveProfiles()) {
            QMessageBox::information(this, tr("Profil Kaydedildi"),
                                    tr("Profil başarıyla güncellendi: %1").arg(currentProfile));
        } else {
            QMessageBox::warning(this, tr("Hata"),
                                tr("Profil güncellenemedi: %1").arg(currentProfile));
        }
    }
}

void MouseTab::onResetToDefaultsClicked()
{
    if (QMessageBox::question(this, tr("Varsayılana Sıfırla"),
                            tr("Tüm ayarları varsayılan değerlere sıfırlamak istediğinizden emin misiniz?"),
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        
        if (isConnected && mouseController) {
            mouseController->resetToDefaults();
            
            // Arayüzü güncelle
            updateUiFromProfile();
            
            QMessageBox::information(this, tr("Sıfırlama Tamamlandı"),
                                    tr("Tüm ayarlar varsayılan değerlere sıfırlandı."));
        }
    }
}