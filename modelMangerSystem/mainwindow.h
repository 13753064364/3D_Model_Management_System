#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedLayout>

class LoginPage;
class ForgotPasswordPage;
class StoreSelectionPage;
class HomePage;

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

private:
    void setupUI();
    void applyFramelessFullScreen();

private:
    QStackedLayout *stackLayout;
    LoginPage *loginPage;
    ForgotPasswordPage *forgotPasswordPage;
    StoreSelectionPage *storeSelectionPage;
    HomePage *homePage;
};
#endif // MAINWINDOW_H
