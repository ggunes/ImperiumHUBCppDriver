#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QObject>
#include <QString>
#include <QTranslator>
#include <QApplication>
#include <QSettings>

class LanguageManager : public QObject
{
    Q_OBJECT

public:
    static LanguageManager* instance();
    
    void setLanguage(const QString &language);
    QString currentLanguage() const;
    
    QStringList availableLanguages() const;

signals:
    void languageChanged(const QString &language);

private:
    explicit LanguageManager(QObject *parent = nullptr);
    ~LanguageManager();
    
    void loadLanguage(const QString &language);
    
    QString m_currentLanguage;
    QTranslator m_translator;
    static LanguageManager* m_instance;
};

#endif // LANGUAGEMANAGER_H