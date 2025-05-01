#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QFile>
#include <QTextStream>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgileri
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarı
    QString language = settings.value("language", "tr").toString();
    QTranslator translator;
    if (language != "en") {
        QString translationFile = QString(":/translations/imperiumhub_%1.qm").arg(language);
        if (translator.load(translationFile)) {
            app.installTranslator(&translator);
        }
    }
    
    // Tema ayarı
    QString theme = settings.value("theme", "system").toString();
    QString styleSheet;
    QFile styleFile;
    
    if (theme == "light") {
        styleFile.setFileName(":/styles/light.qss");
    } else if (theme == "dark") {
        styleFile.setFileName(":/styles/dark.qss");
    } else {
        // Sistem teması
        if (app.style()->objectName().contains("dark", Qt::CaseInsensitive)) {
            styleFile.setFileName(":/styles/dark.qss");
        } else {
            styleFile.setFileName(":/styles/light.qss");
        }
    }
    
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        styleSheet = stream.readAll();
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }
    
    // Ana pencereyi oluştur
    MainWindow mainWindow;
    
    // Başlangıç ayarı
    bool startMinimized = settings.value("startMinimized", false).toBool();
    if (startMinimized) {
        mainWindow.hide();
    } else {
        mainWindow.show();
    }
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgilerini ayarla
    app.setApplicationName("ImperiumHUB");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImperiumGear");
    app.setOrganizationDomain("imperiumgear.com");
    
    // Ayarları yükle
    QSettings settings;
    
    // Dil ayarını yükle
    QString language = settings.value("appearance/language", QLocale::system().name().left(2)).toString();
    
    // Çevirici oluştur
    QTranslator translator;
    if (language == "tr") {
        // Türkçe çeviri dosyasını yükle
        if (translator.load(":/translations/imperiumhub_tr.qrc")) {
            app.installTranslator(&translator);
        }
    }
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int arg