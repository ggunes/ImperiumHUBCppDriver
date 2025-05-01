// ... existing code ...

bool ProfileManager::exportProfile(const QString &profileName, const QString &filePath)
{
    // Profil var mı kontrol et
    if (!profiles.contains(profileName)) {
        return false;
    }
    
    // Profil verilerini al
    QJsonObject profile = profiles[profileName].toObject();
    
    // Dışa aktarma için JSON oluştur
    QJsonObject exportData;
    exportData["profile"] = profile;
    exportData["name"] = profileName;
    exportData["type"] = static_cast<int>(getProfileType(profileName));
    exportData["version"] = "1.0";
    
    QJsonDocument doc(exportData);
    
    // Dosyaya kaydet
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    return true;
}

bool ProfileManager::importProfile(const QString &filePath)
{
    // Dosyayı aç
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    // JSON verisini oku
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }
    
    QJsonObject importData = doc.object();
    
    // Gerekli alanları kontrol et
    if (!importData.contains("profile") || !importData.contains("name") || !importData.contains("type")) {
        return false;
    }
    
    QString profileName = importData["name"].toString();
    ProfileType type = static_cast<ProfileType>(importData["type"].toInt());
    QJsonObject profile = importData["profile"].toObject();
    
    // Aynı isimde profil var mı kontrol et
    if (profiles.contains(profileName)) {
        // Benzersiz isim oluştur
        int counter = 1;
        QString newName = profileName;
        while (profiles.contains(newName)) {
            newName = QString("%1 (%2)").arg(profileName).arg(counter++);
        }
        profileName = newName;
    }
    
    // Profili ekle
    profiles[profileName] = profile;
    
    // Profil türünü kaydet
    if (type == ProfileType::Keyboard) {
        keyboardProfiles.append(profileName);
    } else if (type == ProfileType::Mouse) {
        mouseProfiles.append(profileName);
    }
    
    // Profilleri kaydet
    saveProfiles();
    
    return true;
}