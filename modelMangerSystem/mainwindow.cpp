#include "mainwindow.h"
#include "loginpage.h"
#include "forgotpasswordpage.h"
#include "storeselectionpage.h"
#include "homepage.h"
#include "networkmanager.h"
#include "configmanager.h"

#include <QWidget>
#include <QStackedLayout>
#include <QMessageBox>
#include <QJsonObject>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , networkManager(new NetworkManager(this))
    , configManager(new ConfigManager(this))
    , loginSuccessTimer(new QTimer(this))
{
    setupUI();
    applyFramelessFullScreen();
    

    connect(networkManager, &NetworkManager::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(networkManager, &NetworkManager::loginFailed, this, &MainWindow::onLoginFailed);
    connect(networkManager, &NetworkManager::userInfoReceived, this, &MainWindow::onUserInfoReceived);
    connect(networkManager, &NetworkManager::userInfoFailed, this, &MainWindow::onUserInfoFailed);
    connect(networkManager, &NetworkManager::connectionTestResult, this, &MainWindow::onConnectionTestResult);
    connect(networkManager, &NetworkManager::storeListReceived, this, &MainWindow::onStoreListReceived);
    

    networkManager->testConnection();
    

    if (!networkManager->isSslSupported()) {
        qDebug() << "警告：系统不支持SSL，将使用HTTP连接";
    }
    

    loginSuccessTimer->setSingleShot(true);
    loginSuccessTimer->setInterval(1000);
    connect(loginSuccessTimer, &QTimer::timeout, this, &MainWindow::onLoginSuccessWithDelay);
    

    loadSavedAccount();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    stackLayout = new QStackedLayout(central);

    loginPage = new LoginPage(this);
    forgotPasswordPage = new ForgotPasswordPage(this);
    storeSelectionPage = new StoreSelectionPage(this);
    homePage = new HomePage(this);

    stackLayout->addWidget(loginPage);
    stackLayout->addWidget(forgotPasswordPage);
    stackLayout->addWidget(storeSelectionPage);
    stackLayout->addWidget(homePage);

    connect(loginPage, &LoginPage::forgotPasswordRequested, this, &MainWindow::showForgotPasswordPage);
    connect(loginPage, &LoginPage::loginRequested, this, &MainWindow::handleLogin);
    connect(forgotPasswordPage, &ForgotPasswordPage::backToLoginRequested, this, &MainWindow::showLoginPage);
    connect(storeSelectionPage, &StoreSelectionPage::storeEntered, this, &MainWindow::handleStoreEntered);

    stackLayout->setCurrentWidget(loginPage);
    
    loginPage->setBackgroundImage(":/images/images/background.jpg");
    loginPage->setLogos(":/images/images/logo_small.png", ":/images/images/logo_big.png");
}

void MainWindow::applyFramelessFullScreen()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    showFullScreen();
}

void MainWindow::showForgotPasswordPage()
{
    stackLayout->setCurrentWidget(forgotPasswordPage);
}

void MainWindow::showLoginPage()
{
    stackLayout->setCurrentWidget(loginPage);
}

void MainWindow::handleLogin(const QString &username, const QString &password, bool rememberAccount)
{
    if (username.trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请输入用户名"));
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请输入密码"));
        return;
    }
    

    currentUsername = username.trimmed();
    

    configManager->saveAccount(username.trimmed(), password, rememberAccount);
    

    networkManager->login(username.trimmed(), password);
    
    qDebug() << "开始登录请求，用户名:" << username.trimmed();
}

void MainWindow::handleStoreEntered(int storeId)
{
    homePage->setUserInfo(tr("用户ID: %1").arg(storeId));
    homePage->setHeaderLogo(":/images/images/logo_small.png");
    homePage->setUserAvatar("");
    stackLayout->setCurrentWidget(homePage);
}


void MainWindow::onLoginSuccess(const QJsonObject &tokenInfo)
{
    qDebug() << "登录成功，Token信息:" << tokenInfo;
    loginPage->setLoginStatus("已登录", "#67C23A");
    loginSuccessTimer->start();
    networkManager->getUserInfo();
}

void MainWindow::onLoginFailed(const QString &errorMessage)
{
    qDebug() << "登录失败:" << errorMessage;
    QMessageBox::warning(this, tr("登录失败"), tr("登录失败: %1").arg(errorMessage));
}

void MainWindow::onUserInfoReceived(const QJsonObject &userInfo)
{
    qDebug() << "获取用户信息成功:" << userInfo;
    QJsonObject user = userInfo.value("user").toObject();
    QJsonObject shopInfo = userInfo.value("shopInfo").toObject();
    
    QString userName = user.value("userName").toString();
    QString shopName = shopInfo.value("shopName").toString();
    
    currentUsername = userName;
    currentShopName = shopName;
    
    qDebug() << "当前登录用户:" << userName << "，归属门店:" << shopName;

}

void MainWindow::onUserInfoFailed(const QString &errorMessage)
{
    qDebug() << "获取用户信息失败:" << errorMessage;
    QMessageBox::warning(this, tr("获取用户信息失败"), tr("获取用户信息失败: %1").arg(errorMessage));
}

void MainWindow::onConnectionTestResult(bool success, const QString &message)
{
    qDebug() << "网络连接测试结果:" << success << message;
    
    if (success) {
        qDebug() << "网络连接正常，可以开始登录";
        if (message.contains("网络连接正常")) {
            qDebug() << "HTTP连接成功，现在测试HTTPS连接";
            networkManager->testHttpsWithKnownSite();
        } else if (message.contains("HTTPS连接正常")) {
            qDebug() << "HTTPS连接正常，现在测试目标服务器连接";
            networkManager->testHttpsConnection();
        } else if (message.contains("目标服务器连接正常")) {
            qDebug() << "目标服务器连接也正常，可以开始登录";
        }
    } else {
        if (message.contains("目标服务器")) {
            QMessageBox::warning(this, tr("服务器连接问题"), 
                tr("目标服务器连接失败: %1\n\n请检查：\n1. 服务器地址是否正确\n2. 网络防火墙设置\n3. 代理服务器配置").arg(message));
        } else {
            QMessageBox::warning(this, tr("网络连接问题"), 
                tr("网络连接测试失败: %1\n请检查网络连接后重试").arg(message));
        }
    }
}

void MainWindow::onStoreListReceived(const QStringList &storeNames, const QStringList &storeLocations)
{
    qDebug() << "收到门店列表，共" << storeNames.size() << "个门店";
    qDebug() << "门店名称列表:" << storeNames;
    qDebug() << "门店位置列表:" << storeLocations;
    storeSelectionPage->updateStoreList(storeNames, storeLocations);
    qDebug() << "门店列表更新完成";
}

void MainWindow::onLoginSuccessWithDelay()
{
    qDebug() << "1秒延迟后跳转到门店选择页面";
    stackLayout->setCurrentWidget(storeSelectionPage);
}

void MainWindow::loadSavedAccount()
{
    if (configManager->isRememberAccount()) {
        QString savedUsername = configManager->getSavedUsername();
        QString savedPassword = configManager->getSavedPassword();
        
        if (!savedUsername.isEmpty()) {
            qDebug() << "加载保存的账号信息:" << savedUsername;
            loginPage->setSavedAccount(savedUsername, savedPassword, true);
        }
    }
}
