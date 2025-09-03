#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject *parent = nullptr);

    void saveAccount(const QString &username, const QString &password, bool remember);
    QString getSavedUsername() const;
    QString getSavedPassword() const;
    bool isRememberAccount() const;
    void clearAccount();

    void setValue(const QString &key, const QVariant &value);
    QVariant getValue(const QString &key, const QVariant &defaultValue = QVariant()) const;

private:
    QSettings *settings;
    static const QString USERNAME_KEY;
    static const QString PASSWORD_KEY;
    static const QString REMEMBER_KEY;
};

#endif // CONFIGMANAGER_H
