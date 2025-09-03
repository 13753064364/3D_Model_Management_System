#ifndef FORGOTPASSWORDPAGE_H
#define FORGOTPASSWORDPAGE_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;

class ForgotPasswordPage : public QWidget
{
    Q_OBJECT
public:
    explicit ForgotPasswordPage(QWidget *parent = nullptr);

signals:
    void backToLoginRequested();

private slots:
    void onBackToLoginClicked();

private:
    void setupUI();
    void applyStyles();

private:
    QLabel *titleLabel;
    QLabel *loginTitle;
    QLabel *forgotTitle;
    QLabel *phoneLabel;
    QLineEdit *phoneEdit;
    QLabel *codeLabel;
    QLineEdit *codeEdit;
    QPushButton *getCodeBtn;
    QLabel *pwdLabel;
    QLineEdit *pwdEdit;
    QPushButton *saveBtn;
    QPushButton *cancelBtn;
    QLabel *backLogin;
};

#endif // FORGOTPASSWORDPAGE_H


