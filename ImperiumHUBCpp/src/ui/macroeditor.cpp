#include "macroeditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QInputDialog>

MacroEditor::MacroEditor(ProfileManager *profileManager, QWidget *parent)
    : QDialog(parent),
      profileManager(profileManager)
{
    setupUi();
    
    // Makro listesini güncelle
    updateMacroList();
    
    // Pencere ayarları
    setWindowTitle(tr("Makro Düzenleyici"));
    setMinimumSize(600, 500);
}

void MacroEditor::setupUi()
{
    // Ana düzen
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Üst panel - Makro listesi ve adımlar
    QHBoxLayout *topLayout = new QHBoxLayout();
    
    // Makro listesi
    QGroupBox *macroListGroup = new QGroupBox(tr("Makrolar"), this);
    QVBoxLayout *macroListLayout = new QVBoxLayout(macroListGroup);
    
    macroListWidget = new QListWidget(this);
    macroListLayout->addWidget(macroListWidget);
    
    QHBoxLayout *macroButtonsLayout = new QHBoxLayout();
    
    newMacroButton = new QPushButton(tr("Yeni"), this);
    macroButtonsLayout->addWidget(newMacroButton);
    
    deleteMacroButton = new QPushButton(tr("Sil"), this);
    macroButtonsLayout->addWidget(deleteMacroButton);
    
    renameMacroButton = new QPushButton(tr("Yeniden Adlandır"), this);
    macroButtonsLayout->addWidget(renameMacroButton);
    
    macroListLayout->addLayout(macroButtonsLayout);
    
    topLayout->addWidget(macroListGroup);
    
    // Makro adımları
    QGroupBox *stepsGroup = new QGroupBox(tr("Makro Adımları"), this);
    QVBoxLayout *stepsLayout = new QVBoxLayout(stepsGroup);
    
    stepsListWidget = new QListWidget(this);
    stepsLayout->addWidget(stepsListWidget);
    
    QHBoxLayout *stepsButtonsLayout = new QHBoxLayout();
    
    addStepButton = new QPushButton(tr("Ekle"), this);
    stepsButtonsLayout->addWidget(addStepButton);
    
    editStepButton = new QPushButton(tr("Düzenle"), this);
    stepsButtonsLayout->addWidget(editStepButton);
    
    deleteStepButton = new QPushButton(tr("Sil"), this);
    stepsButtonsLayout->addWidget(deleteStepButton);
    
    moveUpButton = new QPushButton(tr("Yukarı"), this);
    stepsButtonsLayout->addWidget(moveUpButton);
    
    moveDownButton = new QPushButton(tr("Aşağı"), this);
    stepsButtonsLayout->addWidget(moveDownButton);
    
    // Makro kaydetme butonu ekle
    recordMacroButton = new QPushButton(tr("Makro Kaydet"), this);
    stepsButtonsLayout->addWidget(recordMacroButton);
    
    stepsLayout->addLayout(stepsButtonsLayout);
    
    topLayout->addWidget(stepsGroup);
    
    mainLayout->addLayout(topLayout);
    
    // Alt panel - Makro ayarları
    QGroupBox *settingsGroup = new QGroupBox(tr("Makro Ayarları"), this);
    QGridLayout *settingsLayout = new QGridLayout(settingsGroup);
    
    settingsLayout->addWidget(new QLabel(tr("Döngü Sayısı:"), this), 0, 0);
    loopCountSpin = new QSpinBox(this);
    loopCountSpin->setRange(1, 999);
    loopCountSpin->setValue(1);
    settingsLayout->addWidget(loopCountSpin, 0, 1);
    
    settingsLayout->addWidget(new QLabel(tr("Gecikme Türü:"), this), 1, 0);
    delayTypeCombo = new QComboBox(this);
    delayTypeCombo->addItem(tr("Sabit"));
    delayTypeCombo->addItem(tr("Rastgele"));
    delayTypeCombo->addItem(tr("Yok"));
    settingsLayout->addWidget(delayTypeCombo, 1, 1);
    
    fixedDelayLabel = new QLabel(tr("Sabit Gecikme (ms):"), this);
    settingsLayout->addWidget(fixedDelayLabel, 2, 0);
    fixedDelaySpin = new QSpinBox(this);
    fixedDelaySpin->setRange(0, 10000);
    fixedDelaySpin->setValue(100);
    settingsLayout->addWidget(fixedDelaySpin, 2, 1);
    
    randomDelayMinLabel = new QLabel(tr("Min Gecikme (ms):"), this);
    settingsLayout->addWidget(randomDelayMinLabel, 3, 0);
    randomDelayMinSpin = new QSpinBox(this);
    randomDelayMinSpin->setRange(0, 10000);
    randomDelayMinSpin->setValue(50);
    settingsLayout->addWidget(randomDelayMinSpin, 3, 1);
    
    randomDelayMaxLabel = new QLabel(tr("Max Gecikme (ms):"), this);
    settingsLayout->addWidget(randomDelayMaxLabel, 4, 0);
    randomDelayMaxSpin = new QSpinBox(this);
    randomDelayMaxSpin->setRange(0, 10000);
    randomDelayMaxSpin->setValue(150);
    settingsLayout->addWidget(randomDelayMaxSpin, 4, 1);
    
    mainLayout->addWidget(settingsGroup);
    
    // Butonlar
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    saveButton = new QPushButton(tr("Kaydet"), this);
    buttonLayout->addWidget(saveButton);
    
    cancelButton = new QPushButton(tr("İptal"), this);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Bağlantıları kur
    connect(macroListWidget, &QListWidget::currentRowChanged,
            this, &MacroEditor::onMacroSelected);
    connect(newMacroButton, &QPushButton::clicked,
            this, &MacroEditor::onNewMacroClicked);
    connect(deleteMacroButton, &QPushButton::clicked,
            this, &MacroEditor::onDeleteMacroClicked);
    connect(renameMacroButton, &QPushButton::clicked,
            this, &MacroEditor::onRenameMacroClicked);
    
    connect(addStepButton, &QPushButton::clicked,
            this, &MacroEditor::onAddStepClicked);
    connect(editStepButton, &QPushButton::clicked,
            this, &MacroEditor::onEditStepClicked);
    connect(deleteStepButton, &QPushButton::clicked,
            this, &MacroEditor::onDeleteStepClicked);
    connect(moveUpButton, &QPushButton::clicked,
            this, &MacroEditor::onMoveUpClicked);
    connect(moveDownButton, &QPushButton::clicked,
            this, &MacroEditor::onMoveDownClicked);
    
    connect(loopCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MacroEditor::onLoopCountChanged);
    connect(delayTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MacroEditor::onDelayTypeChanged);
    connect(fixedDelaySpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MacroEditor::onFixedDelayChanged);
    connect(randomDelayMinSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MacroEditor::onRandomDelayMinChanged);
    connect(randomDelayMaxSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MacroEditor::onRandomDelayMaxChanged);
    
    connect(saveButton, &QPushButton::clicked,
            this, &MacroEditor::onSaveClicked);
    connect(cancelButton, &QPushButton::clicked,
            this, &MacroEditor::onCancelClicked);
    
    // Başlangıçta gecikme ayarlarını güncelle
    onDelayTypeChanged(delayTypeCombo->currentIndex());
}

void MacroEditor::setCurrentMacro(const QString& macroName)
{
    currentMacro = macroName;
    
    // Makro listesinde seçili makroyu güncelle
    for (int i = 0; i < macroListWidget->count(); i++) {
        if (macroListWidget->item(i)->text() == macroName) {
            macroListWidget->setCurrentRow(i);
            break;
        }
    }
    
    // Makro adımlarını ve ayarlarını güncelle
    updateMacroSteps();
    updateMacroSettings();
}

void MacroEditor::updateMacroList()
{
    // Mevcut seçimi hatırla
    QString selectedMacro;
    if (macroListWidget->currentItem()) {
        selectedMacro = macroListWidget->currentItem()->text();
    }
    
    // Listeyi temizle ve yeniden doldur
    macroListWidget->clear();
    macroListWidget->addItems(profileManager->getMacroNames());
    
    // Önceki seçimi geri yükle
    if (!selectedMacro.isEmpty()) {
        for (int i = 0; i < macroListWidget->count(); i++) {
            if (macroListWidget->item(i)->text() == selectedMacro) {
                macroListWidget->setCurrentRow(i);
                break;
            }
        }
    }
    
    // Butonları güncelle
    bool hasMacros = macroListWidget->count() > 0;
    deleteMacroButton->setEnabled(hasMacros);
    renameMacroButton->setEnabled(hasMacros);
}

void MacroEditor::updateMacroSteps()
{
    stepsListWidget->clear();
    
    if (currentMacro.isEmpty() || !macroListWidget->currentItem()) {
        // Makro seçili değilse adım listesini devre dışı bırak
        stepsListWidget->setEnabled(false);
        addStepButton->setEnabled(false);
        editStepButton->setEnabled(false);
        deleteStepButton->setEnabled(false);
        moveUpButton->setEnabled(false);
        moveDownButton->setEnabled(false);
        return;
    }
    
    // Makro adımlarını al ve listele
    stepsListWidget->setEnabled(true);
    addStepButton->setEnabled(true);
    
    QJsonArray steps = profileManager->getMacroSteps(currentMacro);
    for (int i = 0; i < steps.size(); i++) {
        QJsonObject step = steps[i].toObject();
        QString stepType = step["type"].toString();
        QString stepText;
        
        if (stepType == "keydown") {
            stepText = tr("Tuş Basma: %1").arg(step["key"].toString());
        } else if (stepType == "keyup") {
            stepText = tr("Tuş Bırakma: %1").arg(step["key"].toString());
        } else if (stepType == "delay") {
            stepText = tr("Gecikme: %1 ms").arg(step["time"].toInt());
        } else if (stepType == "mousedown") {
            stepText = tr("Fare Basma: %1").arg(step["button"].toString());
        } else if (stepType == "mouseup") {
            stepText = tr("Fare Bırakma: %1").arg(step["button"].toString());
        } else if (stepType == "mousemove") {
            stepText = tr("Fare Hareketi: X=%1, Y=%2").arg(step["x"].toInt()).arg(step["y"].toInt());
        }
        
        stepsListWidget->addItem(stepText);
    }
    
    // Adım butonlarını güncelle
    bool hasSteps = stepsListWidget->count() > 0;
    editStepButton->setEnabled(hasSteps);
    deleteStepButton->setEnabled(hasSteps);
    moveUpButton->setEnabled(hasSteps && stepsListWidget->currentRow() > 0);
    moveDownButton->setEnabled(hasSteps && stepsListWidget->currentRow() < stepsListWidget->count() - 1);
}

void MacroEditor::updateMacroSettings()
{
    if (currentMacro.isEmpty() || !macroListWidget->currentItem()) {
        // Makro seçili değilse ayarları devre dışı bırak
        loopCountSpin->setEnabled(false);
        delayTypeCombo->setEnabled(false);
        fixedDelaySpin->setEnabled(false);
        randomDelayMinSpin->setEnabled(false);
        randomDelayMaxSpin->setEnabled(false);
        return;
    }
    
    // Makro ayarlarını al ve arayüzü güncelle
    loopCountSpin->setEnabled(true);
    delayTypeCombo->setEnabled(true);
    
    QJsonObject settings = profileManager->getMacroSettings(currentMacro);
    loopCountSpin->setValue(settings["loopCount"].toInt(1));
    
    QString delayType = settings["delayType"].toString("fixed");
    if (delayType == "fixed") {
        delayTypeCombo->setCurrentIndex(0);
        fixedDelaySpin->setValue(settings["fixedDelay"].toInt(100));
    } else if (delayType == "random") {
        delayTypeCombo->setCurrentIndex(1);
        randomDelayMinSpin->setValue(settings["randomDelayMin"].toInt(50));
        randomDelayMaxSpin->setValue(settings["randomDelayMax"].toInt(150));
    } else {
        delayTypeCombo->setCurrentIndex(2);
    }
}

void MacroEditor::onMacroSelected(int index)
{
    if (index >= 0 && index < macroListWidget->count()) {
        currentMacro = macroListWidget->item(index)->text();
        updateMacroSteps();
        updateMacroSettings();
    } else {
        currentMacro = "";
        stepsListWidget->clear();
    }
}

void MacroEditor::onNewMacroClicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Yeni Makro"),
                                         tr("Makro adı:"), QLineEdit::Normal,
                                         tr("Yeni Makro"), &ok);
    if (ok && !name.isEmpty()) {
        // Aynı isimde makro var mı kontrol et
        QStringList macros = profileManager->getMacroNames();
        if (macros.contains(name)) {
            QMessageBox::warning(this, tr("Hata"),
                                tr("Bu isimde bir makro zaten var."));
            return;
        }
        
        // Yeni makro oluştur
        if (profileManager->createMacro(name)) {
            updateMacroList();
            
            // Yeni makroyu seç
            for (int i = 0; i < macroListWidget->count(); i++) {
                if (macroListWidget->item(i)->text() == name) {
                    macroListWidget->setCurrentRow(i);
                    break;
                }
            }
        }
    }
}

void MacroEditor::onDeleteMacroClicked()
{
    if (currentMacro.isEmpty() || !macroListWidget->currentItem()) {
        return;
    }
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Makro Sil"),
                                 tr("'%1' makrosunu silmek istediğinizden emin misiniz?").arg(currentMacro),
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (profileManager->deleteMacro(currentMacro)) {
            updateMacroList();
        }
    }
}

void MacroEditor::onRenameMacroClicked()
{
    if (currentMacro.isEmpty() || !macroListWidget->currentItem()) {
        return;
    }
    
    bool ok;
    QString newName = QInputDialog::getText(this, tr("Makro Yeniden Adlandır"),
                                           tr("Yeni makro adı:"), QLineEdit::Normal,
                                           currentMacro, &ok);
    
    if (ok && !newName.isEmpty() && newName != currentMacro) {
        // Aynı isimde makro var mı kontrol et
        QStringList macros = profileManager->getMacroNames();
        if (macros.contains(newName)) {
            QMessageBox::warning(this, tr("Hata"),
                                tr("Bu isimde bir makro zaten var."));
            return;
        }
        
        // Makroyu yeniden adlandır
        if (profileManager->renameMacro(currentMacro, newName)) {
            currentMacro = newName;
            updateMacroList();
        }
    }
}

void MacroEditor::onAddStepClicked()
{
    if (currentMacro.isEmpty()) {
        return;
    }
    
    // Makro adımı ekleme diyaloğu
    // Bu kısım daha karmaşık olabilir, basit bir örnek:
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Makro Adımı Ekle"));
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QComboBox *typeCombo = new QComboBox(&dialog);
    typeCombo->addItem(tr("Tuş Basma"));
    typeCombo->addItem(tr("Tuş Bırakma"));
    typeCombo->addItem(tr("Gecikme"));
    typeCombo->addItem(tr("Fare Basma"));
    typeCombo->addItem(tr("Fare Bırakma"));
    typeCombo->addItem(tr("Fare Hareketi"));
    
    layout->addWidget(new QLabel(tr("Adım Türü:"), &dialog));
    layout->addWidget(typeCombo);
    
    QStackedWidget *stackedWidget = new QStackedWidget(&dialog);
    
    // Tuş sayfası
    QWidget *keyPage = new QWidget(&dialog);
    QVBoxLayout *keyLayout = new QVBoxLayout(keyPage);
    QComboBox *keyCombo = new QComboBox(keyPage);
    keyCombo->addItems(QStringList() << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" << "I" << "J" << "K" << "L" << "M" 
                                    << "N" << "O" << "P" << "Q" << "R" << "S" << "T" << "U" << "V" << "W" << "X" << "Y" << "Z"
                                    << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9"
                                    << "F1" << "F2" << "F3" << "F4" << "F5" << "F6" << "F7" << "F8" << "F9" << "F10" << "F11" << "F12"
                                    << "Enter" << "Escape" << "Tab" << "Space" << "Backspace" << "Delete");
    keyLayout->addWidget(new QLabel(tr("Tuş:"), keyPage));
    keyLayout->addWidget(keyCombo);
    stackedWidget->addWidget(keyPage);
    
    // Gecikme sayfası
    QWidget *delayPage = new QWidget(&dialog);
    QVBoxLayout *delayLayout = new QVBoxLayout(delayPage);
    QSpinBox *delaySpinBox = new QSpinBox(delayPage);
    delaySpinBox->setRange(1, 10000);
    delaySpinBox->setValue(100);
    delaySpinBox->setSuffix(" ms");
    delayLayout->addWidget(new QLabel(tr("Gecikme Süresi:"), delayPage));
    delayLayout->addWidget(delaySpinBox);
    stackedWidget->addWidget(delayPage);
    
    // Fare tuşu sayfası
    QWidget *mouseButtonPage = new QWidget(&dialog);
    QVBoxLayout *mouseButtonLayout = new QVBoxLayout(mouseButtonPage);
    QComboBox *mouseButtonCombo = new QComboBox(mouseButtonPage);
    mouseButtonCombo->addItem(tr("Sol Tuş"));
    mouseButtonCombo->addItem(tr("Sağ Tuş"));
    mouseButtonCombo->addItem(tr("Orta Tuş"));
    mouseButtonLayout->addWidget(new QLabel(tr("Fare Tuşu:"), mouseButtonPage));
    mouseButtonLayout->addWidget(mouseButtonCombo);
    stackedWidget->addWidget(mouseButtonPage);
    
    // Fare hareketi sayfası
    QWidget *mouseMoveWidget = new QWidget(&dialog);
    QGridLayout *mouseMoveLayout = new QGridLayout(mouseMoveWidget);
    QSpinBox *xSpinBox = new QSpinBox(mouseMoveWidget);
    QSpinBox *ySpinBox = new QSpinBox(mouseMoveWidget);
    xSpinBox->setRange(-9999, 9999);
    ySpinBox->setRange(-9999, 9999);
    mouseMoveLayout->addWidget(new QLabel(tr("X Koordinatı:"), mouseMoveWidget), 0, 0);
    mouseMoveLayout->addWidget(xSpinBox, 0, 1);
    mouseMoveLayout->addWidget(new QLabel(tr("Y Koordinatı:"), mouseMoveWidget), 1, 0);
    mouseMoveLayout->addWidget(ySpinBox, 1, 1);
    stackedWidget->addWidget(mouseMoveWidget);
    
    layout->addWidget(stackedWidget);
    
    // Sayfa değişimini bağla
    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        if (index == 0 || index == 1) { // Tuş basma/bırakma
            stackedWidget->setCurrentIndex(0);
        } else if (index == 2) { // Gecikme
            stackedWidget->setCurrentIndex(1);
        } else if (index == 3 || index == 4) { // Fare basma/bırakma
            stackedWidget->setCurrentIndex(2);
        } else if (index == 5) { // Fare hareketi
            stackedWidget->setCurrentIndex(3);
        }
    });
    
    // Butonlar
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Seçilen adım türüne göre JSON oluştur
        QJsonObject step;
        int type = typeCombo->currentIndex();
        
        if (type == 0) { // Tuş basma
            step["type"] = "keydown";
            step["key"] = keyCombo->currentText();
        } else if (type == 1) { // Tuş bırakma
            step["type"] = "keyup";
            step["key"] = keyCombo->currentText();
        } else if (type == 2) { // Gecikme
            step["type"] = "delay";
            step["time"] = delaySpinBox->value();
        } else if (type == 3) { // Fare basma
            step["type"] = "mousedown";
            step["button"] = mouseButtonCombo->currentText();
        } else if (type == 4) { // Fare bırakma
            step["type"] = "mouseup";
            step["button"] = mouseButtonCombo->currentText();
        } else if (type == 5) { // Fare hareketi
            step["type"] = "mousemove";
            step["x"] = xSpinBox->value();
            step["y"] = ySpinBox->value();
        }
        
        // Adımı ekle
        if (profileManager->addMacroStep(currentMacro, step)) {
            updateMacroSteps();
        }
    }
}

void MacroEditor::onEditStepClicked()
{
    if (currentMacro.isEmpty() || !stepsListWidget->currentItem()) {
        return;
    }
    
    int stepIndex = stepsListWidget->currentRow();
    QJsonArray steps = profileManager->getMacroSteps(currentMacro);
    
    if (stepIndex < 0 || stepIndex >= steps.size()) {
        return;
    }
    
    // Mevcut adımı al
    QJsonObject step = steps[stepIndex].toObject();
    
    // Adım düzenleme diyaloğu (onAddStepClicked'e benzer)
    // Burada mevcut adım değerleriyle diyaloğu doldurmalıyız
    
    // Basitleştirilmiş örnek:
    if (step["type"].toString() == "delay") {
        bool ok;
        int time = QInputDialog::getInt(this, tr("Gecikme Düzenle"),
                                       tr("Gecikme süresi (ms):"), step["time"].toInt(100),
                                       1, 10000, 1, &ok);
        if (ok) {
            step["time"] = time;
            if (profileManager->updateMacroStep(currentMacro, stepIndex, step)) {
                updateMacroSteps();
            }
        }
    } else {
        QMessageBox::information(this, tr("Düzenleme"),
                                tr("Bu adım türü şu anda düzenlenemiyor."));
    }
}

void MacroEditor::onDeleteStepClicked()
{
    if (currentMacro.isEmpty() || !stepsListWidget->currentItem()) {
        return;
    }
    
    int stepIndex = stepsListWidget->currentRow();
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Adım Sil"),
                                 tr("Seçili adımı silmek istediğinizden emin misiniz?"),
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (profileManager->deleteMacroStep(currentMacro, stepIndex)) {
            updateMacroSteps();
        }
    }
}

void MacroEditor::onMoveUpClicked()
{
    if (currentMacro.isEmpty() || !stepsListWidget->currentItem()) {
        return;
    }
    
    int stepIndex = stepsListWidget->currentRow();
    if (stepIndex <= 0) {
        return;
    }
    
    if (profileManager->moveMacroStep(currentMacro, stepIndex, stepIndex - 1)) {
        updateMacroSteps();
        stepsListWidget->setCurrentRow(stepIndex - 1);
    }
}

void MacroEditor::onMoveDownClicked()
{
    if (currentMacro.isEmpty() || !stepsListWidget->currentItem()) {
        return;
    }
    
    int stepIndex = stepsListWidget->currentRow();
    if (stepIndex >= stepsListWidget->count() - 1) {
        return;
    }
    
    if (profileManager->moveMacroStep(currentMacro, stepIndex, stepIndex + 1)) {
        updateMacroSteps();
        stepsListWidget->setCurrentRow(stepIndex + 1);
    }
}

void MacroEditor::onLoopCountChanged(int value)
{
    if (currentMacro.isEmpty()) {
        return;
    }
    
    QJsonObject settings = profileManager->getMacroSettings(currentMacro);
    settings["loopCount"] = value;
    profileManager->setMacroSettings(currentMacro, settings);
}

void MacroEditor::onDelayTypeChanged(int index)
{
    if (currentMacro.isEmpty()) {
        return;
    }
    
    QJsonObject settings = profileManager->getMacroSettings(currentMacro);
    
    // Gecikme türüne göre arayüzü güncelle
    if (index == 0) { // Sabit
        settings["delayType"] = "fixed";
        fixedDelayLabel->setVisible(true);
        fixedDelaySpin->setVisible(true);
        randomDelayMinLabel->setVisible(false);
        randomDelayMinSpin->setVisible(false);
        randomDelayMaxLabel->setVisible(false);
        randomDelayMaxSpin->setVisible(false);
    } else if (index == 1) { // Rastgele
        settings["delayType"] = "random";
        fixedDelayLabel->setVisible(false);
        fixedDelaySpin->setVisible(false);
        randomDelayMinLabel->setVisible(true);
        randomDelayMinSpin->setVisible(true);
        randomDelayMaxLabel->setVisible(true);
        randomDelayMaxSpin->setVisible(true);
    } else { // Yok
        settings["delayType"] = "none";
        fixedDelayLabel->setVisible(false);
        fixedDelaySpin->setVisible(false);
        randomDelayMinLabel->setVisible(false);
        randomDelayMinSpin->setVisible(false);
        randomDelayMaxLabel->setVisible(false);
        randomDelayMaxSpin->setVisible(false);
    }
    
    profileManager->setMacroSettings(currentMacro, settings);
}

void MacroEditor::onFixedDelayChanged(int value)
{
    if (currentMacro.isEmpty()) {
        return;
    }
    
    QJsonObject settings = profileManager->getMacroSettings(currentMacro);
    settings["fixedDelay"] = value;
    profileManager->setMacroSettings(currentMacro, settings);
}

void MacroEditor::onRandomDelayMinChanged(int value)
{
    if (currentMacro.isEmpty()) {
        return;
    }
    
    QJsonObject settings = profileManager->getMacroSettings(currentMacro);
    settings["randomDelayMin"] = value;
    
    // Minimum değer maksimum değerden büyük olamaz
    if (value > randomDelayMaxSpin->value()) {
        randomDelayMaxSpin->setValue(value);
    }
    
    profileManager->setMacroSettings(currentMacro, settings);
}

void MacroEditor::onRandomDelayMaxChanged(int value)
{
    if (currentMacro.isEmpty()) {
        return;
    }
    
    QJsonObject settings = profileManager->getMacroSettings(currentMacro);
    settings["randomDelayMax"] = value;
    
    // Maksimum değer minimum değerden küçük olamaz
    if (value < randomDelayMinSpin->value()) {
        randomDelayMinSpin->setValue(value);
    }
    
    profileManager->setMacroSettings(currentMacro, settings);
}

void MacroEditor::onSaveClicked()
{
    // Tüm değişiklikleri kaydet
    profileManager->saveProfiles();
    
    // Pencereyi kapat
    accept();
}

void MacroEditor::onCancelClicked()
{
    // Değişiklikleri iptal et ve pencereyi kapat
    reject();
}


void MacroEditor::onRecordMacroClicked()
{
    if (currentMacro.isEmpty()) {
        return;
    }
    
    // Kayıt diyaloğu
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Makro Kaydet"));
    dialog.setMinimumWidth(300);
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QLabel *infoLabel = new QLabel(tr("Makro kaydını başlatmak için 'Başlat' butonuna tıklayın.\n"
                                     "Kayıt sırasında yaptığınız tüm tuş ve fare hareketleri kaydedilecektir.\n"
                                     "Kaydı durdurmak için 'Durdur' butonuna tıklayın."), &dialog);
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);
    
    QLabel *statusLabel = new QLabel(tr("Hazır"), &dialog);
    layout->addWidget(statusLabel);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *startButton = new QPushButton(tr("Başlat"), &dialog);
    QPushButton *stopButton = new QPushButton(tr("Durdur"), &dialog);
    QPushButton *cancelButton = new QPushButton(tr("İptal"), &dialog);
    
    stopButton->setEnabled(false);
    
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(stopButton);
    buttonLayout->addWidget(cancelButton);
    
    layout->addLayout(buttonLayout);
    
    // Makro kaydedici
    MacroRecorder *recorder = new MacroRecorder(&dialog);
    
    // Bağlantılar
    connect(startButton, &QPushButton::clicked, [&]() {
        statusLabel->setText(tr("Kaydediliyor..."));
        startButton->setEnabled(false);
        stopButton->setEnabled(true);
        recorder->startRecording();
    });
    
    connect(stopButton, &QPushButton::clicked, [&]() {
        statusLabel->setText(tr("Kayıt tamamlandı"));
        startButton->setEnabled(true);
        stopButton->setEnabled(false);
        recorder->stopRecording();
        
        // Kaydedilen adımları göster
        QJsonArray steps = recorder->getRecordedSteps();
        statusLabel->setText(tr("Kayıt tamamlandı. %1 adım kaydedildi.").arg(steps.size()));
    });
    
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Kaydedilen adımları ekle
        QJsonArray steps = recorder->getRecordedSteps();
        for (int i = 0; i < steps.size(); i++) {
            profileManager->addMacroStep(currentMacro, steps[i].toObject());
        }
        
        // Adım listesini güncelle
        updateMacroSteps();
    }
    
    delete recorder;
}