#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();


    void login(const QString &username, const QString &password);
    void getUserInfo();
    void logout();
    void testConnection();
    void testHttpsConnection();
    void testHttpsWithKnownSite();


    QString getAccessToken() const;
    QString getTokenHead() const;
    bool isLoggedIn() const;
    

    bool isSslSupported() const;

signals:
    void loginSuccess(const QJsonObject &tokenInfo);
    void loginFailed(const QString &errorMessage);
    void userInfoReceived(const QJsonObject &userInfo);
    void userInfoFailed(const QString &errorMessage);
    void logoutSuccess();
    void logoutFailed(const QString &errorMessage);
    void connectionTestResult(bool success, const QString &message);
    void storeListReceived(const QStringList &storeNames, const QStringList &storeLocations);

private slots:
    void onLoginFinished();
    void onUserInfoFinished();
    void onLogoutFinished();
    void onNetworkError(QNetworkReply::NetworkError error);

private:
    QNetworkAccessManager *networkManager;
    QNetworkReply *currentReply;
    

    QString accessToken;
    QString tokenHead;
    bool loggedIn;
    bool sslSupported;
    

    void processUserInfoResponse(const QByteArray &data);
    

    static const QString BASE_URL;
    static const QString LOGIN_URL;
    static const QString GET_INFO_URL;
    static const QString LOGOUT_URL;
    
    // 辅助方法
    void clearCurrentReply();
    QNetworkRequest createRequest(const QString &url);
    QNetworkRequest createAuthenticatedRequest(const QString &url);
    void handleNetworkError(QNetworkReply *reply, const QString &operation);
};

#endif // NETWORKMANAGER_H
