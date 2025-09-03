#include "networkmanager.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QApplication>


const QString NetworkManager::BASE_URL = "https://www.lihevitality.com";
const QString NetworkManager::LOGIN_URL = BASE_URL + "/test/face-api/desktop/user/login";
const QString NetworkManager::GET_INFO_URL = BASE_URL + "/test/face-api/desktop/user/getInfo";
const QString NetworkManager::LOGOUT_URL = BASE_URL + "/test/face-api/desktop/user/oauth/logout";

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , networkManager(new QNetworkAccessManager(this))
    , currentReply(nullptr)
    , loggedIn(false)
    , sslSupported(false)
{
    qDebug() << "检查SSL支持状态:";
    qDebug() << "  SSL支持:" << QSslSocket::supportsSsl();
    qDebug() << "  SSL版本:" << QSslSocket::sslLibraryVersionString();
    qDebug() << "  SSL构建版本:" << QSslSocket::sslLibraryBuildVersionString();
    
    sslSupported = QSslSocket::supportsSsl();
    if (!sslSupported) {
        qDebug() << "警告：系统不支持SSL，HTTPS连接将失败";
        qDebug() << "建议：安装OpenSSL库或使用支持SSL的Qt版本";
    }
    

    qputenv("QT_SSL_USE_TEMPORARY_KEYCHAIN", "1");
    qputenv("QT_SSL_VERIFY_SERVER_CERTIFICATE", "0");
    qputenv("QT_SSL_IGNORE_ERRORS", "1");
    

    qputenv("OPENSSL_CONF", "D:/env/OpenSSL-Win64/bin/openssl.cfg");
    qputenv("PATH", "D:/env/OpenSSL-Win64/bin;" + qgetenv("PATH"));
    

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
    

    sslConfig.setSslOption(QSsl::SslOptionDisableCompression, false);
    sslConfig.setSslOption(QSsl::SslOptionDisableEmptyFragments, false);
    sslConfig.setSslOption(QSsl::SslOptionDisableLegacyRenegotiation, false);
    sslConfig.setSslOption(QSsl::SslOptionDisableSessionTickets, false);
    sslConfig.setSslOption(QSsl::SslOptionDisableSessionPersistence, false);
    
    QSslConfiguration::setDefaultConfiguration(sslConfig);
    
    qDebug() << "SSL配置已设置，协议版本:" << sslConfig.protocol();
    qDebug() << "SSL验证模式:" << sslConfig.peerVerifyMode();
    qDebug() << "SSL选项已配置";
}

NetworkManager::~NetworkManager()
{
    clearCurrentReply();
}

void NetworkManager::login(const QString &username, const QString &password)
{
    clearCurrentReply();
    

    QString loginUrl = LOGIN_URL;
    if (!sslSupported && loginUrl.startsWith("https://")) {
        loginUrl = loginUrl.replace("https://", "http://");
        qDebug() << "SSL不支持，自动切换到HTTP模式";
        qDebug() << "登录URL已更改为:" << loginUrl;
    }
    
    qDebug() << "准备发送登录请求到:" << loginUrl;
    qDebug() << "用户名:" << username;
    qDebug() << "SSL支持状态:" << sslSupported;
    
    QNetworkRequest request = createRequest(loginUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    
    QUrlQuery postData;
    postData.addQueryItem("userName", username);
    postData.addQueryItem("password", password);
    
    QString postDataString = postData.toString(QUrl::FullyEncoded);
    qDebug() << "POST数据:" << postDataString;
    
    currentReply = networkManager->post(request, postDataString.toUtf8());
    
    if (!currentReply) {
        qDebug() << "错误：无法创建网络请求";
        emit loginFailed("网络请求创建失败");
        return;
    }
    
    connect(currentReply, &QNetworkReply::finished, this, &NetworkManager::onLoginFinished);
    connect(currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, &NetworkManager::onNetworkError);
    
    qDebug() << "登录请求已发送，等待响应...";
}

void NetworkManager::getUserInfo()
{
    if (!loggedIn || accessToken.isEmpty() || tokenHead.isEmpty()) {
        qDebug() << "无法获取用户信息：未登录或Token为空";
        emit userInfoFailed("请先登录获取Token");
        return;
    }
    
    qDebug() << "=== 开始获取用户信息 ===";
    qDebug() << "Token状态 - 已登录:" << loggedIn << "TokenHead:" << tokenHead << "AccessToken长度:" << accessToken.length();
    

    if (tokenHead != "Bearer ") {
        qDebug() << "警告：TokenHead格式不正确，期望'Bearer '，实际:" << tokenHead;
    }
    
    if (accessToken.length() < 100) {
        qDebug() << "警告：AccessToken长度异常，长度:" << accessToken.length();
    }
    
    clearCurrentReply();
    
    QNetworkRequest request = createAuthenticatedRequest(GET_INFO_URL);
    qDebug() << "用户信息请求URL:" << GET_INFO_URL;
    

    QString httpUrl = GET_INFO_URL;
    httpUrl.replace("https://", "http://");
    qDebug() << "测试HTTP版本URL:" << httpUrl;
    

    QNetworkAccessManager *tempNetworkManager = new QNetworkAccessManager(this);
    currentReply = tempNetworkManager->get(request);
    qDebug() << "使用临时网络管理器发送用户信息请求";
    

    QNetworkRequest httpRequest = createAuthenticatedRequest(httpUrl);
    QNetworkReply *httpReply = tempNetworkManager->get(httpRequest);
    qDebug() << "同时发送HTTP版本的用户信息请求";
    

    connect(httpReply, &QNetworkReply::finished, this, [this, httpReply]() {
        qDebug() << "HTTP版本用户信息请求完成";
        if (httpReply->error() == QNetworkReply::NoError) {
            QByteArray data = httpReply->readAll();
            qDebug() << "HTTP版本响应数据:" << data;
            

            if (data.contains("301 Moved Permanently") || data.contains("<title>301")) {
                qDebug() << "检测到301重定向，服务器要求使用HTTPS";
                qDebug() << "尝试使用HTTP协议但配置SSL忽略错误";
                

                QNetworkRequest httpsRequest = createAuthenticatedRequest(GET_INFO_URL);
                QSslConfiguration sslConfig = httpsRequest.sslConfiguration();
                sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
                sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
                sslConfig.setSslOption(QSsl::SslOptionDisableCompression, true);
                sslConfig.setSslOption(QSsl::SslOptionDisableEmptyFragments, true);
                sslConfig.setSslOption(QSsl::SslOptionDisableLegacyRenegotiation, true);
                sslConfig.setSslOption(QSsl::SslOptionDisableSessionTickets, true);
                httpsRequest.setSslConfiguration(sslConfig);
                
                QNetworkAccessManager *httpsNetworkManager = new QNetworkAccessManager(this);
                QNetworkReply *httpsReply = httpsNetworkManager->get(httpsRequest);
                
                connect(httpsReply, &QNetworkReply::finished, this, [this, httpsReply]() {
                    qDebug() << "HTTPS版本用户信息请求完成";
                    if (httpsReply->error() == QNetworkReply::NoError) {
                        QByteArray httpsData = httpsReply->readAll();
                        qDebug() << "HTTPS版本响应数据:" << httpsData;

                        this->processUserInfoResponse(httpsData);
                    } else {
                        qDebug() << "HTTPS版本请求错误:" << httpsReply->errorString();
                        emit userInfoFailed("HTTPS请求失败: " + httpsReply->errorString());
                    }
                    httpsReply->deleteLater();
                });
            }
        } else {
            qDebug() << "HTTP版本请求错误:" << httpReply->errorString();
        }
        httpReply->deleteLater();
    });
    
    if (!currentReply) {
        qDebug() << "错误：无法创建用户信息请求";
        emit userInfoFailed("无法创建网络请求");
        return;
    }
    
    qDebug() << "用户信息请求已创建，QNetworkReply对象:" << currentReply;
    qDebug() << "请求URL:" << currentReply->url().toString();
    
    connect(currentReply, &QNetworkReply::finished, this, &NetworkManager::onUserInfoFinished);
    connect(currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, &NetworkManager::onNetworkError);
    

    connect(currentReply, &QNetworkReply::metaDataChanged, this, [this]() {
        qDebug() << "用户信息请求元数据已更改";
    });
    
    connect(currentReply, &QNetworkReply::redirected, this, [this](const QUrl &url) {
        qDebug() << "用户信息请求被重定向到:" << url.toString();
    });
    

    connect(currentReply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesReceived, qint64 bytesTotal) {
        qDebug() << "用户信息请求下载进度:" << bytesReceived << "/" << bytesTotal;
    });
    
    connect(currentReply, &QNetworkReply::readyRead, this, [this]() {
        qDebug() << "用户信息请求开始接收数据";
    });
    
    qDebug() << "获取用户信息请求已发送，等待响应...";
    

    qDebug() << "立即检查请求状态:";
    qDebug() << "  请求是否正在运行:" << currentReply->isRunning();
    qDebug() << "  请求是否已完成:" << currentReply->isFinished();
    qDebug() << "  请求错误:" << currentReply->error();
    

    QCoreApplication::processEvents();
    qDebug() << "强制处理事件循环后检查请求状态:";
    qDebug() << "  请求是否正在运行:" << currentReply->isRunning();
    qDebug() << "  请求是否已完成:" << currentReply->isFinished();
    qDebug() << "  请求错误:" << currentReply->error();
    

    QTimer::singleShot(1000, this, [this]() {
        if (currentReply) {
            qDebug() << "1秒后检查用户信息请求状态:";
            qDebug() << "  请求是否正在运行:" << currentReply->isRunning();
            qDebug() << "  请求是否已完成:" << currentReply->isFinished();
            qDebug() << "  请求错误:" << currentReply->error();
            

            if (!currentReply->isRunning() && !currentReply->isFinished()) {
                qDebug() << "请求没有运行，尝试重新发送...";
                currentReply->abort();

                QNetworkRequest request = createAuthenticatedRequest(GET_INFO_URL);
                currentReply = networkManager->get(request);
                if (currentReply) {
                    connect(currentReply, &QNetworkReply::finished, this, &NetworkManager::onUserInfoFinished);
                    connect(currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
                            this, &NetworkManager::onNetworkError);
                    qDebug() << "重新发送用户信息请求";
                }
            }
        }
    });
    

    QTimer::singleShot(2000, this, [this]() {
        if (currentReply) {
            qDebug() << "2秒后检查用户信息请求状态:";
            qDebug() << "  请求是否正在运行:" << currentReply->isRunning();
            qDebug() << "  请求是否已完成:" << currentReply->isFinished();
            qDebug() << "  请求错误:" << currentReply->error();
        }
    });
    

    QTimer::singleShot(10000, this, [this]() {
        if (currentReply && currentReply->isRunning()) {
            qDebug() << "用户信息请求超时，取消请求";
            currentReply->abort();
        }
    });
    

    QTimer::singleShot(5000, this, [this]() {
        if (currentReply) {
            qDebug() << "5秒后检查用户信息请求状态:";
            qDebug() << "  请求是否正在运行:" << currentReply->isRunning();
            qDebug() << "  请求是否已完成:" << currentReply->isFinished();
            qDebug() << "  请求错误:" << currentReply->error();
            qDebug() << "  请求URL:" << currentReply->url().toString();
            

            if (currentReply->isRunning()) {
                qDebug() << "请求仍在运行，尝试强制完成...";
                currentReply->abort();
            }
        }
    });
}

void NetworkManager::logout()
{
    if (!loggedIn || accessToken.isEmpty() || tokenHead.isEmpty()) {
        emit logoutFailed("请先登录");
        return;
    }
    
    clearCurrentReply();
    
    QNetworkRequest request = createAuthenticatedRequest(LOGOUT_URL);
    currentReply = networkManager->post(request, QByteArray());
    
    connect(currentReply, &QNetworkReply::finished, this, &NetworkManager::onLogoutFinished);
    connect(currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, &NetworkManager::onNetworkError);
    
    qDebug() << "发送登出请求";
}

void NetworkManager::testConnection()
{
    clearCurrentReply();
    

    QString testUrl = "http://www.baidu.com";
    qDebug() << "测试网络连接到:" << testUrl;
    
    QNetworkRequest request(testUrl);
    request.setRawHeader("User-Agent", "Qt Desktop Application");
    
    currentReply = networkManager->get(request);
    
    if (!currentReply) {
        qDebug() << "错误：无法创建连接测试请求";
        emit connectionTestResult(false, "无法创建网络请求");
        return;
    }
    
    connect(currentReply, &QNetworkReply::finished, [this]() {
        if (!currentReply) return;
        
        int statusCode = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString errorString = currentReply->errorString();
        
        qDebug() << "连接测试完成，状态码:" << statusCode;
        
        if (currentReply->error() == QNetworkReply::NoError) {
            emit connectionTestResult(true, "网络连接正常");
        } else {
            emit connectionTestResult(false, "网络连接失败: " + errorString);
        }
        
        clearCurrentReply();
    });
    
    connect(currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            [this](QNetworkReply::NetworkError error) {
        if (!currentReply) return;
        
        QString errorString = currentReply->errorString();
        qDebug() << "连接测试网络错误:" << error << errorString;
        emit connectionTestResult(false, "网络错误: " + errorString);
        clearCurrentReply();
    });
    
    qDebug() << "连接测试请求已发送";
}

void NetworkManager::testHttpsConnection()
{
    clearCurrentReply();
    
    qDebug() << "测试目标服务器连接到:" << BASE_URL;
    
    QNetworkRequest request = createRequest(BASE_URL);
    currentReply = networkManager->get(request);
    
    if (!currentReply) {
        qDebug() << "错误：无法创建HTTPS连接测试请求";
        emit connectionTestResult(false, "无法创建HTTPS网络请求");
        return;
    }
    
    connect(currentReply, &QNetworkReply::finished, [this]() {
        if (!currentReply) return;
        
        int statusCode = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString errorString = currentReply->errorString();
        
        qDebug() << "目标服务器连接测试完成，状态码:" << statusCode;
        
        if (currentReply->error() == QNetworkReply::NoError) {
            emit connectionTestResult(true, "目标服务器连接正常");
        } else {
            emit connectionTestResult(false, "目标服务器连接失败: " + errorString);
        }
        
        clearCurrentReply();
    });
    
    connect(currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            [this](QNetworkReply::NetworkError error) {
        if (!currentReply) return;
        
        QString errorString = currentReply->errorString();
        qDebug() << "目标服务器连接测试网络错误:" << error << errorString;
        emit connectionTestResult(false, "目标服务器网络错误: " + errorString);
        clearCurrentReply();
    });
    
    qDebug() << "目标服务器连接测试请求已发送";
}

void NetworkManager::testHttpsWithKnownSite()
{
    clearCurrentReply();
    

    QString testUrl = "https://www.baidu.com";
    qDebug() << "测试HTTPS连接到已知网站:" << testUrl;
    
    QNetworkRequest request = createRequest(testUrl);
    currentReply = networkManager->get(request);
    
    if (!currentReply) {
        qDebug() << "错误：无法创建HTTPS测试请求";
        emit connectionTestResult(false, "无法创建HTTPS网络请求");
        return;
    }
    
    connect(currentReply, &QNetworkReply::finished, [this]() {
        if (!currentReply) return;
        
        int statusCode = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString errorString = currentReply->errorString();
        
        qDebug() << "HTTPS测试完成，状态码:" << statusCode;
        
        if (currentReply->error() == QNetworkReply::NoError) {
            emit connectionTestResult(true, "HTTPS连接正常，SSL配置工作正常");
        } else {
            emit connectionTestResult(false, "HTTPS连接失败: " + errorString);
        }
        
        clearCurrentReply();
    });
    
    connect(currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            [this](QNetworkReply::NetworkError error) {
        if (!currentReply) return;
        
        QString errorString = currentReply->errorString();
        qDebug() << "HTTPS测试网络错误:" << error << errorString;
        emit connectionTestResult(false, "HTTPS网络错误: " + errorString);
        clearCurrentReply();
    });
    
    qDebug() << "HTTPS测试请求已发送";
}

QString NetworkManager::getAccessToken() const
{
    return accessToken;
}

QString NetworkManager::getTokenHead() const
{
    return tokenHead;
}

bool NetworkManager::isLoggedIn() const
{
    return loggedIn;
}

bool NetworkManager::isSslSupported() const
{
    return sslSupported;
}

void NetworkManager::onLoginFinished()
{
    if (!currentReply) {
        qDebug() << "错误：currentReply为空";
        return;
    }
    
    int statusCode = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "收到登录响应，HTTP状态码:" << statusCode;
    qDebug() << "响应头:" << currentReply->rawHeaderList();
    

    if (statusCode == 301 || statusCode == 302) {
        QUrl redirectUrl = currentReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        qDebug() << "检测到重定向，目标URL:" << redirectUrl;
        
        if (redirectUrl.isValid()) {

            if (redirectUrl.scheme() == "https" && BASE_URL.startsWith("http://")) {
                qDebug() << "检测到HTTP到HTTPS重定向，需要解决SSL问题";
                emit loginFailed("服务器要求使用HTTPS连接，但当前SSL配置有问题。请检查网络环境或联系管理员。");
            } else {
                qDebug() << "重定向到:" << redirectUrl;
                emit loginFailed("服务器重定向到: " + redirectUrl.toString());
            }
        } else {
            emit loginFailed("服务器返回重定向但未提供目标URL");
        }
        clearCurrentReply();
        return;
    }
    
    QByteArray data = currentReply->readAll();
    qDebug() << "响应数据长度:" << data.length();
    qDebug() << "响应数据内容:" << data;
    
    if (data.isEmpty()) {
        emit loginFailed("服务器返回空响应");
        clearCurrentReply();
        return;
    }
    

    QString responseText = QString::fromUtf8(data);
    if (responseText.contains("<html>") || responseText.contains("301") || responseText.contains("Moved Permanently")) {
        qDebug() << "服务器返回HTML重定向页面，要求使用HTTPS";
        emit loginFailed("服务器要求使用HTTPS连接。当前使用HTTP连接被重定向。\n\n建议：\n1. 检查服务器是否只支持HTTPS\n2. 解决SSL/TLS配置问题\n3. 联系服务器管理员确认协议要求");
        clearCurrentReply();
        return;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误:" << error.errorString();
        qDebug() << "响应内容不是有效的JSON格式";
        emit loginFailed("服务器返回的不是有效的JSON数据。可能是重定向或错误页面。");
        clearCurrentReply();
        return;
    }
    
    QJsonObject result = doc.object();
    qDebug() << "登录响应结果:" << result;
    
    int code = result.value("code").toInt();
    if (code == 200 && result.contains("data")) {
        QJsonObject dataObj = result.value("data").toObject();
        accessToken = dataObj.value("accessToken").toString();
        tokenHead = dataObj.value("tokenHead").toString();
        loggedIn = true;
        
        qDebug() << "登录成功，Token已保存";
        qDebug() << "AccessToken:" << accessToken;
        qDebug() << "TokenHead:" << tokenHead;
        emit loginSuccess(dataObj);
    } else {
        QString errorMsg = result.value("msg").toString();
        if (errorMsg.isEmpty()) {
            errorMsg = "未知错误";
        }
        qDebug() << "登录失败，错误码:" << code << "错误信息:" << errorMsg;
        emit loginFailed(errorMsg);
    }
    
    clearCurrentReply();
}

void NetworkManager::onUserInfoFinished()
{
    if (!currentReply) return;
    
    qDebug() << "=== 开始处理用户信息API响应 ===";
    
    QByteArray data = currentReply->readAll();
    qDebug() << "用户信息API响应数据长度:" << data.length();
    qDebug() << "用户信息API响应原始数据:" << data;
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误:" << error.errorString();
        emit userInfoFailed("JSON解析错误: " + error.errorString());
        return;
    }
    
    QJsonObject result = doc.object();
    qDebug() << "用户信息响应结果:" << result;
    
    int code = result.value("code").toInt();
    qDebug() << "用户信息API响应码:" << code;
    if (code == 200 && result.contains("data")) {
        QJsonObject dataObj = result.value("data").toObject();
        qDebug() << "成功获取用户信息";
        qDebug() << "用户信息数据结构:" << dataObj;
        qDebug() << "数据对象包含的键:" << dataObj.keys();
        

        if (dataObj.contains("shopInfo")) {
            QJsonObject shopInfo = dataObj.value("shopInfo").toObject();
            qDebug() << "获取到门店信息:" << shopInfo;
            

            QString shopName = shopInfo.value("shopName").toString();
            QString shopId = shopInfo.value("shopId").toString();
            QString enterpriseName = shopInfo.value("enterpriseName").toString();
            
            if (!shopName.isEmpty()) {
                QStringList storeNames;
                QStringList storeLocations;

                storeNames.append(shopName);
                storeLocations.append(enterpriseName.isEmpty() ? "未知企业" : enterpriseName);
                
                qDebug() << "添加门店到列表:" << shopName << "企业:" << enterpriseName;
                qDebug() << "准备发送storeListReceived信号，门店数量:" << storeNames.size();

                emit storeListReceived(storeNames, storeLocations);
                qDebug() << "storeListReceived信号已发送";
            } else {
                qDebug() << "门店名称为空，无法添加门店信息";
            }
        } else if (dataObj.contains("shopList") && dataObj.value("shopList").isArray()) {
            QJsonArray shopList = dataObj.value("shopList").toArray();
            qDebug() << "获取到门店列表，共" << shopList.size() << "个门店";

            QStringList storeNames;
            QStringList storeLocations;
            
            for (int i = 0; i < shopList.size(); ++i) {
                QJsonValue shopValue = shopList.at(i);
                QJsonObject shop = shopValue.toObject();
                QString shopName = shop.value("shopName").toString();
                QString shopLocation = shop.value("address").toString();
                
                if (!shopName.isEmpty()) {
                    storeNames.append(shopName);
                    storeLocations.append(shopLocation.isEmpty() ? "未知地址" : shopLocation);
                }
            }

            emit storeListReceived(storeNames, storeLocations);
        } else {
            qDebug() << "用户信息中未找到shopList字段，尝试其他可能的字段名";

            QStringList possibleKeys = {"shops", "stores", "shopList", "storeList", "shopInfo", "storeInfo"};
            for (const QString &key : possibleKeys) {
                if (dataObj.contains(key)) {
                    qDebug() << "找到可能的门店字段:" << key;
                    QJsonValue value = dataObj.value(key);
                    if (value.isArray()) {
                        QJsonArray shopList = value.toArray();
                        qDebug() << "字段" << key << "是数组，包含" << shopList.size() << "个元素";

                        QStringList storeNames;
                        QStringList storeLocations;
                        
                        for (int i = 0; i < shopList.size(); ++i) {
                            QJsonValue shopValue = shopList.at(i);
                            if (shopValue.isObject()) {
                                QJsonObject shop = shopValue.toObject();
                                QString shopName = shop.value("shopName").toString();
                                if (shopName.isEmpty()) {
                                    shopName = shop.value("name").toString();
                                }
                                QString shopLocation = shop.value("address").toString();
                                if (shopLocation.isEmpty()) {
                                    shopLocation = shop.value("location").toString();
                                }
                                
                                if (!shopName.isEmpty()) {
                                    storeNames.append(shopName);
                                    storeLocations.append(shopLocation.isEmpty() ? "未知地址" : shopLocation);
                                }
                            }
                        }
                        
                        if (!storeNames.isEmpty()) {
                            emit storeListReceived(storeNames, storeLocations);
                            break;
                        }
                    }
                }
            }
        }
        
        emit userInfoReceived(dataObj);
    } else {
        QString errorMsg = result.value("msg").toString();
        if (errorMsg.isEmpty()) {
            errorMsg = "未知错误";
        }
        qDebug() << "获取用户信息失败:" << errorMsg;
        emit userInfoFailed(errorMsg);
    }
    
    clearCurrentReply();
}

void NetworkManager::onLogoutFinished()
{
    if (!currentReply) return;
    
    QByteArray data = currentReply->readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        emit logoutFailed("JSON解析错误: " + error.errorString());
        return;
    }
    
    QJsonObject result = doc.object();
    qDebug() << "登出响应结果:" << result;
    
    int code = result.value("code").toInt();
    if (code == 0) {
        accessToken.clear();
        tokenHead.clear();
        loggedIn = false;
        
        qDebug() << "登出成功";
        emit logoutSuccess();
    } else {
        QString errorMsg = result.value("msg").toString();
        if (errorMsg.isEmpty()) {
            errorMsg = "未知错误";
        }
        qDebug() << "登出失败:" << errorMsg;
        emit logoutFailed(errorMsg);
    }
    
    clearCurrentReply();
}

void NetworkManager::onNetworkError(QNetworkReply::NetworkError error)
{
    if (!currentReply) {
        qDebug() << "网络错误：currentReply为空";
        return;
    }
    
    QString errorString = currentReply->errorString();
    QString url = currentReply->url().toString();
    
    qDebug() << "网络错误发生:";
    qDebug() << "  错误代码:" << error;
    qDebug() << "  错误信息:" << errorString;
    qDebug() << "  请求URL:" << url;
    qDebug() << "  HTTP状态码:" << currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (errorString.contains("TLS") || errorString.contains("SSL")) {
        qDebug() << "检测到SSL/TLS错误，尝试使用HTTP替代方案";

        if (url.contains("baidu.com")) {
            qDebug() << "HTTP连接测试失败，网络可能有问题";
        }
    }

    if (url.contains("login")) {
        qDebug() << "登录请求网络错误，发送loginFailed信号";
        emit loginFailed("网络错误: " + errorString);
    } else if (url.contains("getInfo")) {
        qDebug() << "用户信息请求网络错误，发送userInfoFailed信号";
        emit userInfoFailed("网络错误: " + errorString);
    } else if (url.contains("logout")) {
        qDebug() << "登出请求网络错误，发送logoutFailed信号";
        emit logoutFailed("网络错误: " + errorString);
    }
    
    clearCurrentReply();
}

void NetworkManager::clearCurrentReply()
{
    if (currentReply) {

        currentReply->disconnect();

        if (currentReply->isRunning()) {
            currentReply->abort();
        }

        currentReply->deleteLater();
        currentReply = nullptr;
    }
}

void NetworkManager::processUserInfoResponse(const QByteArray &data)
{
    qDebug() << "=== 开始处理用户信息API响应 ===";
    qDebug() << "用户信息API响应数据长度:" << data.length();
    qDebug() << "用户信息API响应原始数据:" << data;
    
    if (data.isEmpty()) {
        qDebug() << "用户信息API响应为空";
        emit userInfoFailed("服务器返回空响应");
        return;
    }
    
    QString responseText = QString::fromUtf8(data);
    qDebug() << "用户信息API响应文本:" << responseText;
    

    if (responseText.contains("<html>") || responseText.contains("301") || responseText.contains("Moved Permanently")) {
        qDebug() << "用户信息API返回HTML重定向页面";
        emit userInfoFailed("服务器返回重定向页面，可能需要使用HTTPS");
        return;
    }
    

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "用户信息API JSON解析错误:" << error.errorString();
        emit userInfoFailed("JSON解析错误: " + error.errorString());
        return;
    }
    
    QJsonObject result = doc.object();
    qDebug() << "用户信息API响应结果:" << result;
    
    int code = result.value("code").toInt();
    qDebug() << "用户信息API响应码:" << code;
    
    if (code == 200 && result.contains("data")) {
        QJsonObject dataObj = result.value("data").toObject();
        qDebug() << "成功获取用户信息";
        qDebug() << "用户信息数据:" << dataObj;

        QStringList storeNames;
        QStringList storeLocations;

        if (dataObj.contains("shopInfo")) {
            QJsonObject shopInfo = dataObj.value("shopInfo").toObject();
            qDebug() << "找到shopInfo对象:" << shopInfo;
            
            QString shopName = shopInfo.value("shopName").toString();
            QString enterpriseName = shopInfo.value("enterpriseName").toString();
            
            if (!shopName.isEmpty()) {
                storeNames.append(shopName);
                storeLocations.append(enterpriseName);
                qDebug() << "门店名称:" << shopName << "位置:" << enterpriseName;
            }
        }

        if (storeNames.isEmpty()) {
            QStringList possibleKeys = {"shops", "stores", "shopList", "storeList", "shopInfo"};
            for (const QString &key : possibleKeys) {
                if (dataObj.contains(key)) {
                    qDebug() << "尝试解析字段:" << key;
                    QJsonValue value = dataObj.value(key);
                    if (value.isObject()) {
                        QJsonObject obj = value.toObject();
                        QString shopName = obj.value("shopName").toString();
                        QString enterpriseName = obj.value("enterpriseName").toString();
                        if (!shopName.isEmpty()) {
                            storeNames.append(shopName);
                            storeLocations.append(enterpriseName);
                            qDebug() << "从" << key << "找到门店:" << shopName << "位置:" << enterpriseName;
                        }
                    } else if (value.isArray()) {
                        QJsonArray arr = value.toArray();
                        for (int i = 0; i < arr.size(); ++i) {
                            QJsonObject obj = arr.at(i).toObject();
                            QString shopName = obj.value("shopName").toString();
                            QString enterpriseName = obj.value("enterpriseName").toString();
                            if (!shopName.isEmpty()) {
                                storeNames.append(shopName);
                                storeLocations.append(enterpriseName);
                                qDebug() << "从" << key << "数组找到门店:" << shopName << "位置:" << enterpriseName;
                            }
                        }
                    }
                }
            }
        }
        
        if (!storeNames.isEmpty()) {
            qDebug() << "成功解析门店信息，门店数量:" << storeNames.size();
            emit storeListReceived(storeNames, storeLocations);
        } else {
            qDebug() << "未找到门店信息";
            emit storeListReceived(QStringList(), QStringList());
        }
        
        emit userInfoReceived(dataObj);
    } else {
        QString errorMsg = result.value("msg").toString();
        qDebug() << "用户信息API返回错误:" << errorMsg;
        emit userInfoFailed("获取用户信息失败: " + errorMsg);
    }
}

QNetworkRequest NetworkManager::createRequest(const QString &url)
{
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Qt Desktop Application");
    

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
    

    sslConfig.setSslOption(QSsl::SslOptionDisableCompression, false);
    sslConfig.setSslOption(QSsl::SslOptionDisableEmptyFragments, false);
    sslConfig.setSslOption(QSsl::SslOptionDisableLegacyRenegotiation, false);
    sslConfig.setSslOption(QSsl::SslOptionDisableSessionTickets, false);
    sslConfig.setSslOption(QSsl::SslOptionDisableSessionPersistence, false);
    
    request.setSslConfiguration(sslConfig);
    
    return request;
}

QNetworkRequest NetworkManager::createAuthenticatedRequest(const QString &url)
{
    QNetworkRequest request = createRequest(url);
    QString authHeader = tokenHead + accessToken;
    request.setRawHeader("Authorization", authHeader.toUtf8());
    
    qDebug() << "创建认证请求:";
    qDebug() << "  URL:" << url;
    qDebug() << "  TokenHead:" << tokenHead;
    qDebug() << "  AccessToken:" << accessToken.left(20) << "...";
    qDebug() << "  Authorization头:" << authHeader.left(30) << "...";
    qDebug() << "  Authorization头完整内容:" << authHeader;
    
    return request;
}

void NetworkManager::handleNetworkError(QNetworkReply *reply, const QString &operation)
{
    if (!reply) return;
    
    QString errorString = reply->errorString();
    qDebug() << operation << "网络错误:" << errorString;
}
