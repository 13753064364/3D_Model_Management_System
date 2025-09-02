#include "mainwindow.h"
#include "loginpage.h"
#include "forgotpasswordpage.h"
#include "storeselectionpage.h"
#include "homepage.h"

#include <QWidget>
#include <QStackedLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    applyFramelessFullScreen();
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
    stackLayout->setCurrentWidget(storeSelectionPage);
}

void MainWindow::handleStoreEntered(int storeId)
{
    homePage->setUserInfo(tr("用户ID: %1").arg(storeId));
    homePage->setHeaderLogo(":/images/images/logo_small.png");
    homePage->setUserAvatar("");
    stackLayout->setCurrentWidget(homePage);
}
