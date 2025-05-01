#include "keyboardview.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

#include "keyboardview.h"

KeyboardView::KeyboardView(QWidget *parent)
    : QWidget(parent),
      selectedKeyIndex(-1),
      keyboardLayout("TR")
{
    // Tuş haritasını oluştur
    createKeyMap();
    
    // Minimum boyut
    setMinimumSize(800, 300);
}

void KeyboardView::setSelectedKey(int keyIndex)
{
    if (keyIndex != selectedKeyIndex) {
        selectedKeyIndex = keyIndex;
        update();
    }
}

int KeyboardView::getSelectedKey() const
{
    return selectedKeyIndex;
}

void KeyboardView::setKeyColor(int keyIndex, const QColor &color)
{
    keyColors[keyIndex] = color;
    update();
}

void KeyboardView::clearKeyColors()
{
    keyColors.clear();
    update();
}

void KeyboardView::setKeyboardLayout(const QString &layout)
{
    keyboardLayout = layout;
    createKeyMap();
    update();
}

QString KeyboardView::getKeyboardLayout() const
{
    return keyboardLayout;
}

void KeyboardView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Arka planı çiz
    painter.fillRect(rect(), QColor(40, 40, 40));
    
    // Tüm tuşları çiz
    for (auto it = keyMap.begin(); it != keyMap.end(); ++it) {
        bool selected = (it.key() == selectedKeyIndex);
        drawKey(painter, it.value(), selected);
    }
}

void KeyboardView::mousePressEvent(QMouseEvent *event)
{
    // Tıklanan tuşu bul
    for (auto it = keyMap.begin(); it != keyMap.end(); ++it) {
        if (it.value().rect.contains(event->pos())) {
            setSelectedKey(it.key());
            emit keySelected(it.key());
            break;
        }
    }
}

void KeyboardView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    
    // Boyut değiştiğinde tuş haritasını güncelle
    createKeyMap();
}

void KeyboardView::createKeyMap()
{
    keyMap.clear();
    
    // Klavye boyutları
    int keyWidth = width() / 15;
    int keyHeight = height() / 6;
    int spacing = 4;
    
    // Standart tuş boyutu
    QSize keySize(keyWidth - spacing, keyHeight - spacing);
    
    // Başlangıç pozisyonu
    int startX = spacing;
    int startY = spacing;
    
    // Örnek bir klavye düzeni (TR-Q)
    // 1. Satır - Fonksiyon tuşları
    for (int i = 0; i < 12; i++) {
        KeyRect key;
        key.rect = QRect(startX + i * keyWidth, startY, keySize.width(), keySize.height());
        key.label = QString("F%1").arg(i + 1);
        key.keyCode = 0x70 + i; // F1-F12 tuş kodları
        keyMap[i] = key;
    }
    
    // 2. Satır - Rakamlar
    QString row2Keys = "1234567890*-";
    for (int i = 0; i < row2Keys.length(); i++) {
        KeyRect key;
        key.rect = QRect(startX + i * keyWidth, startY + keyHeight, keySize.width(), keySize.height());
        key.label = QString(row2Keys[i]);
        key.keyCode = 0x30 + i; // 0-9 tuş kodları
        keyMap[12 + i] = key;
    }
    
    // 3. Satır - Q satırı
    QString row3Keys = "qwertyuıopğü";
    for (int i = 0; i < row3Keys.length(); i++) {
        KeyRect key;
        key.rect = QRect(startX + i * keyWidth, startY + 2 * keyHeight, keySize.width(), keySize.height());
        key.label = QString(row3Keys[i]).toUpper();
        key.keyCode = 0x41 + i; // A-Z tuş kodları
        keyMap[24 + i] = key;
    }
    
    // 4. Satır - A satırı
    QString row4Keys = "asdfghjklşi,";
    for (int i = 0; i < row4Keys.length(); i++) {
        KeyRect key;
        key.rect = QRect(startX + (i + 0.5) * keyWidth, startY + 3 * keyHeight, keySize.width(), keySize.height());
        key.label = QString(row4Keys[i]).toUpper();
        key.keyCode = 0x41 + i + 12; // A-Z tuş kodları devamı
        keyMap[36 + i] = key;
    }
    
    // 5. Satır - Z satırı
    QString row5Keys = "zxcvbnmöç.";
    for (int i = 0; i < row5Keys.length(); i++) {
        KeyRect key;
        key.rect = QRect(startX + (i + 1) * keyWidth, startY + 4 * keyHeight, keySize.width(), keySize.height());
        key.label = QString(row5Keys[i]).toUpper();
        key.keyCode = 0x41 + i + 24; // A-Z tuş kodları devamı
        keyMap[48 + i] = key;
    }
    
    // Space tuşu
    KeyRect spaceKey;
    spaceKey.rect = QRect(startX + 3 * keyWidth, startY + 5 * keyHeight, 6 * keyWidth - spacing, keySize.height());
    spaceKey.label = "SPACE";
    spaceKey.keyCode = 0x20; // Space tuş kodu
    keyMap[58] = spaceKey;
}

void KeyboardView::drawKey(QPainter &painter, const KeyRect &key, bool selected)
{
    // Tuş rengi
    QColor keyColor;
    
    if (keyColors.contains(selectedKeyIndex) && selected) {
        // Seçili tuş için özel renk
        keyColor = keyColors[selectedKeyIndex];
    } else if (keyColors.contains(key.keyCode)) {
        // Tuş için özel renk
        keyColor = keyColors[key.keyCode];
    } else {
        // Varsayılan tuş rengi
        keyColor = selected ? QColor(100, 100, 255) : QColor(60, 60, 60);
    }
    
    // Tuş arka planı
    painter.setPen(Qt::NoPen);
    painter.setBrush(keyColor);
    painter.drawRoundedRect(key.rect, 5, 5);
    
    // Tuş etiketi
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));
    painter.drawText(key.rect, Qt::AlignCenter, key.label);
}
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

KeyboardView::KeyboardView(QWidget *parent)
    : QWidget(parent),
      selectedKeyIndex(-1),
      keySize(40),
      keySpacing(5),
      keyRadius(5)
{
    // Klavye düzenini oluştur
    setupKeyboardLayout();
    
    // Minimum boyut ayarla
    setMinimumSize(800, 300);
}

void KeyboardView::setupKeyboardLayout()
{
    // Standart ANSI klavye düzeni
    // Bu basit bir örnek, gerçek bir klavye düzeni daha karmaşık olacaktır
    
    // Fonksiyon tuşları (F1-F12)
    for (int i = 0; i < 12; i++) {
        KeyInfo key;
        key.keyIndex = i;
        key.keyName = QString("F%1").arg(i + 1);
        key.color = Qt::white;
        key.isSelected = false;
        keys[i] = key;
    }
    
    // Rakam tuşları
    QString numKeys = "`1234567890-=";
    for (int i = 0; i < numKeys.length(); i++) {
        KeyInfo key;
        key.keyIndex = 12 + i;
        key.keyName = QString(numKeys[i]);
        key.color = Qt::white;
        key.isSelected = false;
        keys[12 + i] = key;
    }
    
    // Üst sıra tuşları
    QString topRowKeys = "QWERTYUIOP[]\\";
    for (int i = 0; i < topRowKeys.length(); i++) {
        KeyInfo key;
        key.keyIndex = 25 + i;
        key.keyName = QString(topRowKeys[i]);
        key.color = Qt::white;
        key.isSelected = false;
        keys[25 + i] = key;
    }
    
    // Orta sıra tuşları
    QString middleRowKeys = "ASDFGHJKL;'";
    for (int i = 0; i < middleRowKeys.length(); i++) {
        KeyInfo key;
        key.keyIndex = 39 + i;
        key.keyName = QString(middleRowKeys[i]);
        key.color = Qt::white;
        key.isSelected = false;
        keys[39 + i] = key;
    }
    
    // Alt sıra tuşları
    QString bottomRowKeys = "ZXCVBNM,./";
    for (int i = 0; i < bottomRowKeys.length(); i++) {
        KeyInfo key;
        key.keyIndex = 50 + i;
        key.keyName = QString(bottomRowKeys[i]);
        key.color = Qt::white;
        key.isSelected = false;
        keys[50 + i] = key;
    }
    
    // Özel tuşlar
    struct {
        int index;
        QString name;
    } specialKeys[] = {
        {60, "Backspace"},
        {61, "Tab"},
        {62, "Caps Lock"},
        {63, "Enter"},
        {64, "Shift"},
        {65, "Ctrl"},
        {66, "Win"},
        {67, "Alt"},
        {68, "Space"},
        {69, "Alt Gr"},
        {70, "Menu"},
        {71, "Ctrl"},
        {72, "Shift"}
    };
    
    for (const auto& sk : specialKeys) {
        KeyInfo key;
        key.keyIndex = sk.index;
        key.keyName = sk.name;
        key.color = Qt::white;
        key.isSelected = false;
        keys[sk.index] = key;
    }
    
    // Tuş konumlarını güncelle
    updateKeyPositions();
}

void KeyboardView::updateKeyPositions()
{
    // Bu fonksiyon, tuşların konumlarını hesaplar
    // Gerçek bir uygulamada, bu daha karmaşık olacaktır
    
    int x = keySpacing;
    int y = keySpacing;
    
    // Fonksiyon tuşları (F1-F12)
    for (int i = 0; i < 12; i++) {
        keys[i].rect = QRect(x, y, keySize, keySize);
        x += keySize + keySpacing;
    }
    
    // Rakam tuşları
    x = keySpacing;
    y += keySize + keySpacing * 2;
    for (int i = 12; i < 25; i++) {
        keys[i].rect = QRect(x, y, keySize, keySize);
        x += keySize + keySpacing;
    }
    
    // Üst sıra tuşları
    x = keySpacing + keySize / 2;
    y += keySize + keySpacing;
    for (int i = 25; i < 39; i++) {
        keys[i].rect = QRect(x, y, keySize, keySize);
        x += keySize + keySpacing;
    }
    
    // Orta sıra tuşları
    x = keySpacing + keySize;
    y += keySize + keySpacing;
    for (int i = 39; i < 50; i++) {
        keys[i].rect = QRect(x, y, keySize, keySize);
        x += keySize + keySpacing;
    }
    
    // Alt sıra tuşları
    x = keySpacing + keySize * 1.5;
    y += keySize + keySpacing;
    for (int i = 50; i < 60; i++) {
        keys[i].rect = QRect(x, y, keySize, keySize);
        x += keySize + keySpacing;
    }
    
    // Özel tuşlar - basit yerleşim
    // Gerçek bir uygulamada, bu daha karmaşık olacaktır
    keys[60].rect = QRect(13 * keySize, keySpacing * 3 + keySize, keySize * 2, keySize); // Backspace
    keys[61].rect = QRect(keySpacing, keySpacing * 4 + keySize * 2, keySize * 1.5, keySize); // Tab
    keys[62].rect = QRect(keySpacing, keySpacing * 5 + keySize * 3, keySize * 1.75, keySize); // Caps Lock
    keys[63].rect = QRect(13 * keySize, keySpacing * 5 + keySize * 3, keySize * 2, keySize); // Enter
    keys[64].rect = QRect(keySpacing, keySpacing * 6 + keySize * 4, keySize * 2.25, keySize); // Left Shift
    keys[65].rect = QRect(keySpacing, keySpacing * 7 + keySize * 5, keySize * 1.5, keySize); // Left Ctrl
    keys[66].rect = QRect(keySpacing * 2 + keySize * 1.5, keySpacing * 7 + keySize * 5, keySize * 1.25, keySize); // Win
    keys[67].rect = QRect(keySpacing * 3 + keySize * 2.75, keySpacing * 7 + keySize * 5, keySize * 1.25, keySize); // Alt
    keys[68].rect = QRect(keySpacing * 4 + keySize * 4, keySpacing * 7 + keySize * 5, keySize * 5, keySize); // Space
    keys[69].rect = QRect(keySpacing * 5 + keySize * 9, keySpacing * 7 + keySize * 5, keySize * 1.25, keySize); // Alt Gr
    keys[70].rect = QRect(keySpacing * 6 + keySize * 10.25, keySpacing * 7 + keySize * 5, keySize * 1.25, keySize); // Menu
    keys[71].rect = QRect(keySpacing * 7 + keySize * 11.5, keySpacing * 7 + keySize * 5, keySize * 1.5, keySize); // Right Ctrl
    keys[72].rect = QRect(keySpacing * 6 + keySize * 10.75, keySpacing * 6 + keySize * 4, keySize * 2.25, keySize); // Right Shift
}

void KeyboardView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Arka planı çiz
    painter.fillRect(rect(), QColor(40, 40, 40));
    
    // Tuşları çiz
    for (const KeyInfo& key : keys) {
        // Tuş arka planı
        QColor keyColor = key.color;
        if (key.keyIndex == selectedKeyIndex) {
            keyColor = QColor(100, 150, 255); // Seçili tuş rengi
        }
        
        painter.setPen(Qt::NoPen);
        painter.setBrush(keyColor);
        painter.drawRoundedRect(key.rect, keyRadius, keyRadius);
        
        // Tuş çerçevesi
        painter.setPen(QPen(QColor(80, 80, 80), 1));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(key.rect, keyRadius, keyRadius);
        
        // Tuş metni
        painter.setPen(Qt::black);
        painter.drawText(key.rect, Qt::AlignCenter, key.keyName);
    }
}

void KeyboardView::mousePressEvent(QMouseEvent *event)
{
    int keyIndex = findKeyAtPosition(event->pos());
    if (keyIndex >= 0) {
        // Önceki seçimi temizle
        if (selectedKeyIndex >= 0 && keys.contains(selectedKeyIndex)) {
            keys[selectedKeyIndex].isSelected = false;
        }
        
        // Yeni tuşu seç
        selectedKeyIndex = keyIndex;
        keys[keyIndex].isSelected = true;
        
        // Güncelle ve sinyal gönder
        update();
        emit keySelected(keyIndex);
    }
}

void KeyboardView::resizeEvent(QResizeEvent *event)
{
    // Boyut değiştiğinde tuş konumlarını güncelle
    updateKeyPositions();
    QWidget::resizeEvent(event);
}

int KeyboardView::findKeyAtPosition(const QPoint& pos)
{
    for (auto it = keys.begin(); it != keys.end(); ++it) {
        if (it.value().rect.contains(pos)) {
            return it.key();
        }
    }
    return -1;
}

void KeyboardView::setKeyColor(int keyIndex, const QColor& color)
{
    if (keys.contains(keyIndex)) {
        keys[keyIndex].color = color;
        update();
    }
}

void KeyboardView::setAllKeysColor(const QColor& color)
{
    for (auto it = keys.begin(); it != keys.end(); ++it) {
        it.value().color = color;
    }
    update();
}

void KeyboardView::clearSelection()
{
    if (selectedKeyIndex >= 0 && keys.contains(selectedKeyIndex)) {
        keys[selectedKeyIndex].isSelected = false;
        selectedKeyIndex = -1;
        update();
    }