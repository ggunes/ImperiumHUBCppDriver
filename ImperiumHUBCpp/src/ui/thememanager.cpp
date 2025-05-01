#include "thememanager.h"
#include <QFile>
#include <QApplication>
#include <QSettings>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <QDebug>

ThemeManager* ThemeManager::m_instance = nullptr;

ThemeManager* ThemeManager::instance()
{
    if (!m_instance) {
        m_instance = new ThemeManager();
    }
    return m_instance;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent),
      m_currentTheme(System)
{
    // Ayarlardan tema yükle
    QSettings settings;
    QString themeStr = settings.value("theme", "system").toString();
    setTheme(stringToTheme(themeStr));
}

ThemeManager::~ThemeManager()
{
    m_instance = nullptr;
}

void ThemeManager::setTheme(Theme theme)
{
    if (m_currentTheme != theme) {
        m_currentTheme = theme;
        applyTheme(theme);
        emit themeChanged(theme);
    }
}

ThemeManager::Theme ThemeManager::currentTheme() const
{
    return m_currentTheme;
}

QString ThemeManager::themeToString(Theme theme) const
{
    switch (theme) {
        case Light: return "light";
        case Dark: return "dark";
        case System: return "system";
        default: return "system";
    }
}

ThemeManager::Theme ThemeManager::stringToTheme(const QString &themeStr) const
{
    if (themeStr == "light") return Light;
    if (themeStr == "dark") return Dark;
    return System;
}

void ThemeManager::applyTheme(Theme theme)
{
    // Sistem teması için işletim sistemi temasını kontrol et
    if (theme == System) {
        // Windows'ta koyu tema kontrolü
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
        bool darkTheme = settings.value("AppsUseDarkTheme", 0).toBool();
        
        theme = darkTheme ? Dark : Light;
    }
    
    // Tema dosyasını yükle
    if (theme == Dark) {
        loadStyleSheet(":/styles/dark.qss");
    } else {
        loadStyleSheet(":/styles/light.qss");
    }
}

void ThemeManager::loadStyleSheet(const QString &path)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    } else {
        qWarning() << "Tema dosyası yüklenemedi:" << path;
    }
}
#include <QApplication>
#include <QSettings>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>

ThemeManager* ThemeManager::m_instance = nullptr;

ThemeManager* ThemeManager::instance()
{
    if (!m_instance) {
        m_instance = new ThemeManager();
    }
    return m_instance;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent),
      m_currentTheme(System)
{
    // Ayarlardan tema yükle
    QSettings settings;
    QString themeName = settings.value("appearance/theme", "system").toString();
    setTheme(themeName);
}

ThemeManager::~ThemeManager()
{
    m_instance = nullptr;
}

void ThemeManager::setTheme(Theme theme)
{
    if (m_currentTheme != theme) {
        m_currentTheme = theme;
        
        // Temayı uygula
        if (theme == System) {
            applySystemTheme();
        } else {
            QString styleSheet = getStyleSheet();
            qApp->setStyleSheet(styleSheet);
        }
        
        // Ayarları güncelle
        QSettings settings;
        QString themeName;
        
        switch (theme) {
            case Light:
                themeName = "light";
                break;
            case Dark:
                themeName = "dark";
                break;
            case System:
                themeName = "system";
                break;
        }
        
        settings.setValue("appearance/theme", themeName);
        
        // Sinyal gönder
        emit themeChanged(theme);
    }
}

void ThemeManager::setTheme(const QString &themeName)
{
    if (themeName == "light") {
        setTheme(Light);
    } else if (themeName == "dark") {
        setTheme(Dark);
    } else {
        setTheme(System);
    }
}

ThemeManager::Theme ThemeManager::currentTheme() const
{
    return m_currentTheme;
}

QString ThemeManager::getStyleSheet() const
{
    switch (m_currentTheme) {
        case Light:
            return loadStyleSheet(":/styles/light.qss");
        case Dark:
            return loadStyleSheet(":/styles/dark.qss");
        case System:
            return "";
    }
    
    return "";
}

QString ThemeManager::loadStyleSheet(const QString &fileName) const
{
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        return QString::fromUtf8(file.readAll());
    }
    
    return "";
}

void ThemeManager::applySystemTheme()
{
    // Sistem temasını uygula
    qApp->setStyle(QStyleFactory::create("Fusion"));
    
    // Sistem temasına göre renkleri ayarla
    QPalette palette = qApp->palette();
    
    // Sistem teması koyu mu?
    bool isDarkMode = false;
    
    // Windows'ta koyu tema kontrolü
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    if (settings.contains("AppsUseLightTheme")) {
        isDarkMode = settings.value("AppsUseLightTheme").toInt() == 0;
    }
    
    if (isDarkMode) {
        // Koyu tema renkleri
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
    } else {
        // Açık tema renkleri
        palette.setColor(QPalette::Window, QColor(240, 240, 240));
        palette.setColor(QPalette::WindowText, Qt::black);
        palette.setColor(QPalette::Base, QColor(255, 255, 255));
        palette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
        palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, Qt::black);
        palette.setColor(QPalette::Button, QColor(240, 240, 240));
        palette.setColor(QPalette::ButtonText, Qt::black);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(0, 0, 255));
        palette.setColor(QPalette::Highlight, QColor(0, 120, 215));
        palette.setColor(QPalette::HighlightedText, Qt::white);
    }
    
    qApp->setPalette(palette);
    qApp->setStyleSheet("");
}