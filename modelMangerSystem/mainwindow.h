#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedLayout>
#include <QMessageBox>

class LoginPage;
class ForgotPasswordPage;
class StoreSelectionPage;
class HomePage;
class NetworkManager;
class ConfigManager;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showForgotPasswordPage();
    void showLoginPage();
    void handleLogin(const QString &username, const QString &password, bool rememberAccount);
    void handleStoreEntered(int storeId);
    

    void onLoginSuccess(const QJsonObject &tokenInfo);
    void onLoginFailed(const QString &errorMessage);
    void onUserInfoReceived(const QJsonObject &userInfo);
    void onUserInfoFailed(const QString &errorMessage);
    void onConnectionTestResult(bool success, const QString &message);
    void onStoreListReceived(const QStringList &storeNames, const QStringList &storeLocations);
    void onLoginSuccessWithDelay();

private:
    void setupUI();
    void applyFramelessFullScreen();
    void loadSavedAccount();

private:
    QStackedLayout *stackLayout;
    LoginPage *loginPage;
    ForgotPasswordPage *forgotPasswordPage;
    StoreSelectionPage *storeSelectionPage;
    HomePage *homePage;
    NetworkManager *networkManager;
    ConfigManager *configManager;
    QTimer *loginSuccessTimer;
    

    QString currentUsername;
    QString currentShopName;
};
#endif // MAINWINDOW_H
