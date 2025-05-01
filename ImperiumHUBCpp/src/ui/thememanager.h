#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QApplication>
#include <QFile>
#include <QSettings>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum Theme {
        Light,
        Dark,
        System
    };
    Q_ENUM(Theme)

    static ThemeManager* instance();
    
    void setTheme(Theme theme);
    Theme currentTheme() const;
    
    QString themeToString(Theme theme) const;
    Theme stringToTheme(const QString &themeStr) const;

signals:
    void themeChanged(Theme theme);

private:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();
    
    void applyTheme(Theme theme);
    void loadStyleSheet(const QString &path);
    
    Theme m_currentTheme;
    static ThemeManager* m_instance;
};

#endif // THEMEMANAGER_H