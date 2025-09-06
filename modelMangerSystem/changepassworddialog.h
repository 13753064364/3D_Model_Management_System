#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

class QShowEvent;

class ChangePasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePasswordDialog(const QString &username, QWidget *parent = nullptr);
    ~ChangePasswordDialog();

    QString getNewPassword() const;
    QString getConfirmPassword() const;

signals:
    void passwordChanged(const QString &newPassword);

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onPasswordTextChanged();
    void onConfirmPasswordTextChanged();

private:
    void setupUI();
    void applyStyles();
    bool validatePasswords();
    void updateSaveButtonState();

private:
    QWidget *overlayWidget;
    QWidget *dialogWidget;
    
    QLabel *titleLabel;
    QLabel *accountLabel;
    QLineEdit *accountEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordEdit;
    QLabel *confirmPasswordLabel;
    QLineEdit *confirmPasswordEdit;
    
    QPushButton *saveButton;
    QPushButton *cancelButton;
    
    QString currentUsername;
};

#endif // CHANGEPASSWORDDIALOG_H
