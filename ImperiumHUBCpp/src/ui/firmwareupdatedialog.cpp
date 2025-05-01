#include "firmwareupdatedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QNetworkRequest>

FirmwareUpdateDialog::FirmwareUpdateDialog(DeviceManager *deviceManager, QWidget *parent)
    : QDialog(parent),
      deviceManager(deviceManager),
      updateAvailable(false),
      currentReply(nullptr)
{
    setupUi();
    
    // Ağ yöneticisi oluştur
    networkManager = new QNetworkAccessManager(this);
    
    // Pencere ayarları
    setWindowTitle(tr("Yazılım Güncelleme"));
    setMinimumSize(400, 300);
}

void FirmwareUpdateDialog::setupUi()
{
    // Ana düzen
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Cihaz bilgileri
    QGridLayout *infoLayout = new QGridLayout();
    
    infoLayout->addWidget(new QLabel(tr("Cihaz:"), this), 0, 0);
    deviceLabel = new QLabel(this);
    infoLayout->addWidget(deviceLabel, 0, 1);
    
    infoLayout->addWidget(new QLabel(tr("Mevcut Sürüm:"), this), 1, 0);
    currentVersionLabel = new QLabel(this);
    infoLayout->addWidget(currentVersionLabel, 1, 1);
    
    infoLayout->addWidget(new QLabel(tr("En Son Sürüm:"), this), 2, 0);
    latestVersionLabel = new QLabel(this);
    infoLayout->addWidget(latestVersionLabel, 2, 1);
    
    mainLayout->addLayout(infoLayout);
    
    // Durum ve ilerleme
    statusLabel = new QLabel(this);
    mainLayout->addWidget(statusLabel);
    
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setVisible(false);
    mainLayout->addWidget(progressBar);
    
    // Butonlar
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    checkButton = new QPushButton(tr("Güncellemeleri Kontrol Et"), this);
    buttonLayout->addWidget(checkButton);
    
    updateButton = new QPushButton(tr("Güncelle"), this);
    updateButton->setEnabled(false);
    buttonLayout->addWidget(updateButton);
    
    cancelButton = new QPushButton(tr("İptal"), this);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Bağlantıları kur
    connect(checkButton, &QPushButton::clicked, this, &FirmwareUpdateDialog::onCheckForUpdatesClicked);
    connect(updateButton, &QPushButton::clicked, this, &FirmwareUpdateDialog::onUpdateClicked);
    connect(cancelButton, &QPushButton::clicked, this, &FirmwareUpdateDialog::onCancelClicked);
}

void FirmwareUpdateDialog::setDeviceId(const QString &deviceId)
{
    this->deviceId = deviceId;
    
    // Cihaz bilgilerini güncelle
    QString deviceName = deviceManager->getDeviceName(deviceId);
    QString currentVersion = deviceManager->getDeviceFirmwareVersion(deviceId);
    
    deviceLabel->setText(deviceName);
    currentVersionLabel->setText(currentVersion);
    latestVersionLabel->setText(tr("Bilinmiyor"));
    
    // Durum etiketini güncelle
    statusLabel->setText(tr("Güncellemeleri kontrol etmek için tıklayın."));
}

void FirmwareUpdateDialog::onCheckForUpdatesClicked()
{
    checkForUpdates();
}

void FirmwareUpdateDialog::onUpdateClicked()
{
    if (updateAvailable) {
        downloadFirmware(firmwareUrl);
    }
}

void FirmwareUpdateDialog::onCancelClicked()
{
    // İndirme işlemi devam ediyorsa iptal et
    if (currentReply && currentReply->isRunning()) {
        currentReply->abort();
    }
    
    // Pencereyi kapat
    reject();
}

void FirmwareUpdateDialog::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        int progress = static_cast<int>((bytesReceived * 100) / bytesTotal);
        progressBar->setValue(progress);
        statusLabel->setText(tr("Yazılım indiriliyor... %1%").arg(progress));
    }
}

void FirmwareUpdateDialog::onDownloadFinished()
{
    if (currentReply->error() == QNetworkReply::NoError) {
        // İndirilen veriyi kaydet
        QByteArray firmwareData = currentReply->readAll();
        
        // Geçici dizin oluştur
        QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        QDir tempDir(tempPath);
        
        // Firmware dosyasını kaydet
        firmwareFilePath = tempDir.filePath("firmware.bin");
        QFile file(firmwareFilePath);
        
        if (file.open(QIODevice::WriteOnly)) {
            file.write(firmwareData);
            file.close();
            
            statusLabel->setText(tr("Yazılım indirildi. Güncelleme başlatılıyor..."));
            
            // Firmware güncelleme işlemini başlat
            updateFirmware();
        } else {
            statusLabel->setText(tr("Hata: Yazılım dosyası kaydedilemedi."));
            progressBar->setVisible(false);
        }
    } else {
        statusLabel->setText(tr("Hata: Yazılım indirilemedi. %1").arg(currentReply->errorString()));
        progressBar->setVisible(false);
    }
    
    currentReply->deleteLater();
    currentReply = nullptr;
}

void FirmwareUpdateDialog::checkForUpdates()
{
    // Güncelleme kontrolü başlat
    statusLabel->setText(tr("Güncellemeler kontrol ediliyor..."));
    checkButton->setEnabled(false);
    updateButton->setEnabled(false);
    
    // Cihaz bilgilerini al
    QString deviceModel = deviceManager->getDeviceModel(deviceId);
    QString currentVersion = deviceManager->getDeviceFirmwareVersion(deviceId);
    
    // Güncelleme sunucusuna istek gönder
    QUrl url("https://api.imperiumgear.com/firmware/check");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject requestData;
    requestData["deviceId"] = deviceId;
    requestData["model"] = deviceModel;
    requestData["currentVersion"] = currentVersion;
    
    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();
    
    currentReply = networkManager->post(request, data);
    
    connect(currentReply, &QNetworkReply::finished, this, [this]() {
        if (currentReply->error() == QNetworkReply::NoError) {
            QByteArray responseData = currentReply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            
            if (doc.isObject()) {
                QJsonObject response = doc.object();
                
                bool hasUpdate = response["hasUpdate"].toBool(false);
                QString latestVersion = response["latestVersion"].toString();
                
                latestVersionLabel->setText(latestVersion);
                
                if (hasUpdate) {
                    updateAvailable = true;
                    firmwareUrl = response["downloadUrl"].toString();
                    
                    statusLabel->setText(tr("Yeni bir güncelleme mevcut: %1").arg(latestVersion));
                    updateButton->setEnabled(true);
                } else {
                    updateAvailable = false;
                    statusLabel->setText(tr("Cihazınız güncel."));
                    updateButton->setEnabled(false);
                }
            } else {
                statusLabel->setText(tr("Hata: Sunucudan geçersiz yanıt alındı."));
            }
        } else {
            statusLabel->setText(tr("Hata: %1").arg(currentReply->errorString()));
        }
        
        currentReply->deleteLater();
        currentReply = nullptr;
        progressBar->setVisible(false);
    });
    
    connect(currentReply, &QNetworkReply::downloadProgress,
            this, &FirmwareUpdateDialog::onDownloadProgress);
}

void FirmwareUpdateDialog::downloadFirmware(const QString &url)
{
    if (url.isEmpty()) {
        statusLabel->setText(tr("Hata: İndirme URL'si boş."));
        return;
    }
    
    // İndirme işlemini başlat
    statusLabel->setText(tr("Firmware indiriliyor..."));
    progressBar->setVisible(true);
    progressBar->setValue(0);
    
    QNetworkRequest request(QUrl(url));
    currentReply = networkManager->get(request);
    
    connect(currentReply, &QNetworkReply::downloadProgress,
            this, &FirmwareUpdateDialog::onDownloadProgress);
    
    connect(currentReply, &QNetworkReply::finished,
            this, &FirmwareUpdateDialog::onDownloadFinished);
}

void FirmwareUpdateDialog::updateFirmware()
{
    if (firmwareFilePath.isEmpty()) {
        statusLabel->setText(tr("Hata: Firmware dosyası bulunamadı."));
        return;
    }
    
    // Güncelleme işlemini başlat
    bool result = deviceManager->updateDeviceFirmware(deviceId, firmwareFilePath);
    
    if (result) {
        statusLabel->setText(tr("Firmware güncelleme başlatıldı. Lütfen bekleyin..."));
        
        // Güncelleme durumunu izle
        connect(deviceManager, &DeviceManager::firmwareUpdateProgress,
                this, &FirmwareUpdateDialog::onFirmwareUpdateProgress);
        
        connect(deviceManager, &DeviceManager::firmwareUpdateCompleted,
                this, &FirmwareUpdateDialog::onFirmwareUpdateCompleted);
        
        connect(deviceManager, &DeviceManager::firmwareUpdateFailed,
                this, &FirmwareUpdateDialog::onFirmwareUpdateFailed);
    } else {
        statusLabel->setText(tr("Hata: Firmware güncelleme başlatılamadı."));
    }
}

void FirmwareUpdateDialog::onFirmwareUpdateProgress(const QString &deviceId, int progress)
{
    if (this->deviceId == deviceId) {
        progressBar->setValue(progress);
        statusLabel->setText(tr("Firmware güncelleniyor... %1%").arg(progress));
    }
}

void FirmwareUpdateDialog::onFirmwareUpdateCompleted(const QString &deviceId)
{
    if (this->deviceId == deviceId) {
        progressBar->setValue(100);
        statusLabel->setText(tr("Firmware güncelleme tamamlandı."));
        
        QMessageBox::information(this, tr("Güncelleme Tamamlandı"),
                                tr("Firmware başarıyla güncellendi. Cihaz yeniden başlatılacak."));
        
        // Pencereyi kapat
        accept();
    }
}

void FirmwareUpdateDialog::onFirmwareUpdateFailed(const QString &deviceId, const QString &errorMessage)
{
    if (this->deviceId == deviceId) {
        statusLabel->setText(tr("Hata: %1").arg(errorMessage));
        
        QMessageBox::critical(this, tr("Güncelleme Hatası"),
                             tr("Firmware güncellenirken bir hata oluştu: %1").arg(errorMessage));
    }
}

void FirmwareUpdateDialog::checkForUpdates()
{
    // Güncelleme kontrolü başlat
    statusLabel->setText(tr("Güncellemeler kontrol ediliyor..."));
    progressBar->setVisible(true);
    progressBar->setValue(0);
    
    // Cihaz bilgilerini al
    QString deviceName = deviceManager->getDeviceName(deviceId);
    QString currentVersion = deviceManager->getDeviceFirmwareVersion(deviceId);
    
    // Sunucudan güncelleme bilgilerini al
    QNetworkRequest request(QUrl("https://api.imperiumgear.com/firmware/check"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject requestData;
    requestData["deviceId"] = deviceId;
    requestData["deviceName"] = deviceName;
    requestData["currentVersion"] = currentVersion;
    
    QJsonDocument doc(requestData);
    
    currentReply = networkManager->post(request, doc.toJson());
    
    connect(currentReply, &QNetworkReply::finished, this, [this]() {
        if (currentReply->error() == QNetworkReply::NoError) {
            // Yanıtı oku
            QByteArray responseData = currentReply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            
            if (doc.isObject()) {
                QJsonObject response = doc.object();
                
                bool hasUpdate = response["hasUpdate"].toBool(false);
                QString latestVersion = response["latestVersion"].toString();
                
                latestVersionLabel->setText(latestVersion);
                
                if (hasUpdate) {
                    updateAvailable = true;
                    firmwareUrl = response["downloadUrl"].toString();
                    
                    statusLabel->setText(tr("Yeni bir güncelleme mevcut: %1").arg(latestVersion));
                    updateButton->setEnabled(true);
                } else {
                    updateAvailable = false;
                    statusLabel->setText(tr("Cihazınız güncel."));
                    updateButton->setEnabled(false);
                }
            } else {
                statusLabel->setText(tr("Hata: Sunucudan geçersiz yanıt alındı."));
            }
        } else {
            statusLabel->setText(tr("Hata: %1").arg(currentReply->errorString()));
        }
        
        currentReply->deleteLater();
        currentReply = nullptr;
        progressBar->setVisible(false);
    });
    
    connect(currentReply, &QNetworkReply::downloadProgress,
            this, &FirmwareUpdateDialog::onDownloadProgress);
}