#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QGraphicsDropShadowEffect;
class QCheckBox;

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);

    void setBackgroundImage(const QString &imagePath);
    void setLogos(const QString &smallLogoPath, const QString &bigLogoPath);
    void setSavedAccount(const QString &username, const QString &password, bool remember);
    void setLoginStatus(const QString &status, const QString &color = "#67C23A");

signals:
    void forgotPasswordRequested();
    void loginRequested(const QString &username, const QString &password, bool rememberAccount);

private slots:
    void onForgotPasswordClicked();
    void onLoginClicked();
    void onUserAgreementClicked();
    void onPrivacyAgreementClicked();

private:
    void setupUI();
    void applyStyles();
    void buildForgotDialog();
    void applyLanguageTexts();
protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QWidget *cardWidget;
    QWidget *leftPane;
    QWidget *rightPane;
    QLabel *titleLabel;
    QLabel *loginTitle;
    QLabel *userLabel;
    QLineEdit *userEdit;
    QLabel *pwdLabel;
    QLineEdit *pwdEdit;
    QCheckBox *agreementCheck;
    QLabel *userAgreementLabel;
    QLabel *privacyAgreementLabel;
    QCheckBox *rememberCheck;
    QLabel *forgotLabel;
    QPushButton *loginBtn;
    QLabel *bottomIdLabel;

    QLabel *smallLogoLabel;
    QLabel *systemTitleLabel;
    QLabel *bigLogoLabel;

    QPushButton *languageBtn;
    bool isEnglish = false;

    QWidget *overlayWidget;
    QWidget *forgotDialog;
    QLineEdit *fpPhoneEdit;
    QLineEdit *fpCodeEdit;
    QPushButton *fpGetCodeBtn;
    QLineEdit *fpNewPwdEdit;
    QPushButton *fpSaveBtn;
    QPushButton *fpCancelBtn;

    QString backgroundImagePath;
    QString smallLogoPath;
    QString bigLogoPath;
};

#endif // LOGINPAGE_H


