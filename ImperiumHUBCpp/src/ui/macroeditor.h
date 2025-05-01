#ifndef MACROEDITOR_H
#define MACROEDITOR_H

#include <QDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include "../profiles/profilemanager.h"

class MacroEditor : public QDialog
{
    Q_OBJECT

public:
    explicit MacroEditor(ProfileManager *profileManager, QWidget *parent = nullptr);
    
    // Düzenlenecek makroyu ayarla
    void setCurrentMacro(const QString& macroName);

private slots:
    // Makro listesi işlemleri
    void onMacroSelected(int index);
    void onNewMacroClicked();
    void onDeleteMacroClicked();
    void onRenameMacroClicked();
    
    // Makro adımları işlemleri
    void onAddStepClicked();
    void onEditStepClicked();
    void onDeleteStepClicked();
    void onMoveUpClicked();
    void onMoveDownClicked();
    
    // Kaydetme işlemleri
    void onSaveClicked();
    void onCancelClicked();
    
    // Makro ayarları
    void onLoopCountChanged(int value);
    void onDelayTypeChanged(int index);
    void onFixedDelayChanged(int value);
    void onRandomDelayMinChanged(int value);
    void onRandomDelayMaxChanged(int value);

private:
    // Arayüz kurulumu
    void setupUi();
    
    // Makro listesini güncelle
    void updateMacroList();
    
    // Makro adımlarını güncelle
    void updateMacroSteps();
    
    // Makro ayarlarını güncelle
    void updateMacroSettings();
    
    // Profil yöneticisi
    ProfileManager *profileManager;
    
    // Aktif makro
    QString currentMacro;
    
    // Arayüz bileşenleri
    QListWidget *macroListWidget;
    QPushButton *newMacroButton;
    QPushButton *deleteMacroButton;
    QPushButton *renameMacroButton;
    
    QListWidget *stepsListWidget;
    QPushButton *addStepButton;
    QPushButton *editStepButton;
    QPushButton *deleteStepButton;
    QPushButton *moveUpButton;
    QPushButton *moveDownButton;
    
    QSpinBox *loopCountSpin;
    QComboBox *delayTypeCombo;
    QSpinBox *fixedDelaySpin;
    QSpinBox *randomDelayMinSpin;
    QSpinBox *randomDelayMaxSpin;
    QLabel *fixedDelayLabel;
    QLabel *randomDelayMinLabel;
    QLabel *randomDelayMaxLabel;
    
    QPushButton *saveButton;
    QPushButton *cancelButton;
};

#endif // MACROEDITOR_H