#include "languagemanager.h"
#include <QDir>
#include <QDebug>

LanguageManager* LanguageManager::m_instance = nullptr;

LanguageManager* LanguageManager::instance()
{
    if (!m_instance) {
        m_instance = new LanguageManager();
    }
    return m_instance;
}

LanguageManager::LanguageManager(QObject *parent)
    : QObject(parent),
      m_currentLanguage("tr")
{
    // Ayarlardan dil yükle
    QSettings settings;
    QString language = settings.value("language", "tr").toString();
    setLanguage(language);
}

LanguageManager::~LanguageManager()
{
    m_instance = nullptr;
}

void LanguageManager::setLanguage(const QString &language)
{
    if (m_currentLanguage != language) {
        m_currentLanguage = language;
        loadLanguage(language);
        emit languageChanged(language);
    }
}

QString LanguageManager::currentLanguage() const
{
    return m_currentLanguage;
}

QStringList LanguageManager::availableLanguages() const
{
    QStringList languages;
    languages << "tr" << "en";
    return languages;
}

void LanguageManager::loadLanguage(const QString &language)
{
    // Önceki çeviriyi kaldır
    qApp->removeTranslator(&m_translator);
    
    // Yeni çeviriyi yükle
    QString path = QString(":/translations/imperiumhub_%1.qm").arg(language);
    if (m_translator.load(path)) {
        qApp->installTranslator(&m_translator);
    } else {
        qWarning() << "Dil dosyası yüklenemedi:" << path;
    }
}