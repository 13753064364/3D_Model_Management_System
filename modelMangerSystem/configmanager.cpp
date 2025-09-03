#include "configmanager.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

const QString ConfigManager::USERNAME_KEY = "username";
const QString ConfigManager::PASSWORD_KEY = "password";
const QString ConfigManager::REMEMBER_KEY = "remember_account";

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configPath);
    
    QString configFile = configPath + "/config.ini";
    settings = new QSettings(configFile, QSettings::IniFormat, this);
    
    qDebug() << "配置文件路径:" << configFile;
}

void ConfigManager::saveAccount(const QString &username, const QString &password, bool remember)
{
    if (remember) {
        settings->setValue(USERNAME_KEY, username);
        settings->setValue(PASSWORD_KEY, password);
        settings->setValue(REMEMBER_KEY, true);
        qDebug() << "账号信息已保存到配置文件";
    } else {
        settings->remove(USERNAME_KEY);
        settings->remove(PASSWORD_KEY);
        settings->setValue(REMEMBER_KEY, false);
        qDebug() << "账号信息已从配置文件清除";
    }
    
    settings->sync();
}

QString ConfigManager::getSavedUsername() const
{
    return settings->value(USERNAME_KEY, "").toString();
}

QString ConfigManager::getSavedPassword() const
{
    return settings->value(PASSWORD_KEY, "").toString();
}

bool ConfigManager::isRememberAccount() const
{
    return settings->value(REMEMBER_KEY, false).toBool();
}

void ConfigManager::clearAccount()
{
    settings->remove(USERNAME_KEY);
    settings->remove(PASSWORD_KEY);
    settings->setValue(REMEMBER_KEY, false);
    settings->sync();
    qDebug() << "账号信息已清除";
}

void ConfigManager::setValue(const QString &key, const QVariant &value)
{
    settings->setValue(key, value);
    settings->sync();
}

QVariant ConfigManager::getValue(const QString &key, const QVariant &defaultValue) const
{
    return settings->value(key, defaultValue);
}
