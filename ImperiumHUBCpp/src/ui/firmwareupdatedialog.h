#ifndef FIRMWAREUPDATEDIALOG_H
#define FIRMWAREUPDATEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QComboBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "../usb/devicemanager.h"

class FirmwareUpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FirmwareUpdateDialog(DeviceManager *deviceManager, QWidget *parent = nullptr);
    
    void setDeviceId(const QString &deviceId);

private slots:
    void onCheckForUpdatesClicked();
    void onUpdateClicked();
    void onCancelClicked();
    
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();
    
    void onUpdateProgress(int progress);
    void onUpdateFinished(bool success);

private:
    void setupUi();
    void checkForUpdates();
    void downloadFirmware(const QString &url);
    void updateFirmware();
    
    DeviceManager *deviceManager;
    QString deviceId;
    QString firmwareUrl;
    QString firmwareVersion;
    QString firmwareFilePath;
    
    QLabel *deviceLabel;
    QLabel *currentVersionLabel;
    QLabel *latestVersionLabel;
    QLabel *statusLabel;
    QProgressBar *progressBar;
    QPushButton *checkButton;
    QPushButton *updateButton;
    QPushButton *cancelButton;
    
    QNetworkAccessManager *networkManager;
    QNetworkReply *currentReply;
    
    bool updateAvailable;
}; // Noktalı virgül eklenmeli

#endif // FIRMWAREUPDATEDIALOG_H