#ifndef KEYBOARDVIEW_H
#define KEYBOARDVIEW_H

#include <QWidget>
#include <QMap>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>

class KeyboardView : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardView(QWidget *parent = nullptr);
    
    // Tuş seçimi
    void setSelectedKey(int keyIndex);
    int getSelectedKey() const;
    
    // Tuş renkleri
    void setKeyColor(int keyIndex, const QColor &color);
    void clearKeyColors();
    
    // Klavye düzeni
    void setKeyboardLayout(const QString &layout);
    QString getKeyboardLayout() const;

signals:
    void keySelected(int keyIndex);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // Tuş koordinatları ve boyutları
    struct KeyRect {
        QRect rect;
        QString label;
        int keyCode;
    };
    
    // Tuş haritası
    QMap<int, KeyRect> keyMap;
    
    // Tuş renkleri
    QMap<int, QColor> keyColors;
    
    // Seçili tuş
    int selectedKeyIndex;
    
    // Klavye düzeni
    QString keyboardLayout;
    
    // Tuş haritasını oluştur
    void createKeyMap();
    
    // Tuş çiz
    void drawKey(QPainter &painter, const KeyRect &key, bool selected);
};

#endif // KEYBOARDVIEW_H