#include "changepassworddialog.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QDebug>
#include <QRegExpValidator>
#include <QShowEvent>

ChangePasswordDialog::ChangePasswordDialog(const QString &username, QWidget *parent)
    : QDialog(parent)
    , overlayWidget(nullptr)
    , dialogWidget(nullptr)
    , titleLabel(nullptr)
    , accountLabel(nullptr)
    , accountEdit(nullptr)
    , passwordLabel(nullptr)
    , passwordEdit(nullptr)
    , confirmPasswordLabel(nullptr)
    , confirmPasswordEdit(nullptr)
    , saveButton(nullptr)
    , cancelButton(nullptr)
    , currentUsername(username)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setModal(true);
    
    setupUI();
    applyStyles();
    
    // 设置账号输入框为只读
    accountEdit->setReadOnly(true);
    accountEdit->setText(currentUsername);
}

ChangePasswordDialog::~ChangePasswordDialog()
{
}

void ChangePasswordDialog::setupUI()
{
    // 设置对话框大小
    setFixedSize(400, 350);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);
    
    // 标题
    titleLabel = new QLabel("修改密码");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    // 账号输入区域
    QVBoxLayout *accountLayout = new QVBoxLayout();
    accountLayout->setSpacing(8);
    accountLabel = new QLabel("账号");
    accountLabel->setObjectName("fieldLabel");
    accountEdit = new QLineEdit();
    accountEdit->setObjectName("accountEdit");
    accountEdit->setPlaceholderText("请输入账号");
    accountLayout->addWidget(accountLabel);
    accountLayout->addWidget(accountEdit);
    
    // 密码输入区域
    QVBoxLayout *passwordLayout = new QVBoxLayout();
    passwordLayout->setSpacing(8);
    passwordLabel = new QLabel("密码");
    passwordLabel->setObjectName("fieldLabel");
    passwordEdit = new QLineEdit();
    passwordEdit->setObjectName("passwordEdit");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("请输入新密码");
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(passwordEdit);
    
    // 确认密码输入区域
    QVBoxLayout *confirmPasswordLayout = new QVBoxLayout();
    confirmPasswordLayout->setSpacing(8);
    confirmPasswordLabel = new QLabel("确认密码");
    confirmPasswordLabel->setObjectName("fieldLabel");
    confirmPasswordEdit = new QLineEdit();
    confirmPasswordEdit->setObjectName("confirmPasswordEdit");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setPlaceholderText("请再次输入新密码");
    confirmPasswordLayout->addWidget(confirmPasswordLabel);
    confirmPasswordLayout->addWidget(confirmPasswordEdit);
    
    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);
    buttonLayout->addStretch();
    
    saveButton = new QPushButton("保存");
    saveButton->setObjectName("saveButton");
    saveButton->setFixedSize(80, 36);
    saveButton->setEnabled(false);
    
    cancelButton = new QPushButton("取消");
    cancelButton->setObjectName("cancelButton");
    cancelButton->setFixedSize(80, 36);
    
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    // 添加到主布局
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(accountLayout);
    mainLayout->addLayout(passwordLayout);
    mainLayout->addLayout(confirmPasswordLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号
    connect(saveButton, &QPushButton::clicked, this, &ChangePasswordDialog::onSaveClicked);
    connect(cancelButton, &QPushButton::clicked, this, &ChangePasswordDialog::onCancelClicked);
    connect(passwordEdit, &QLineEdit::textChanged, this, &ChangePasswordDialog::onPasswordTextChanged);
    connect(confirmPasswordEdit, &QLineEdit::textChanged, this, &ChangePasswordDialog::onConfirmPasswordTextChanged);
}

void ChangePasswordDialog::applyStyles()
{
    setStyleSheet(R"(
        QDialog {
            background-color: #ffffff;
            border: 2px solid #d0d0d0;
            font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
        }
        
        QLabel#titleLabel {
            font-size: 20px;
            font-weight: bold;
            color: #333333;
            margin-bottom: 10px;
        }
        
        QLabel#fieldLabel {
            font-size: 14px;
            color: #666666;
            font-weight: 500;
        }
        
        QLineEdit#accountEdit {
            background-color: #ffffff;
            border: 1px solid #d0d0d0;
            border-radius: 0px;
            padding: 8px 12px;
            font-size: 14px;
            color: #333333;
        }
        
        QLineEdit#passwordEdit, QLineEdit#confirmPasswordEdit {
            background-color: #ffffff;
            border: 1px solid #d0d0d0;
            border-radius: 0px;
            padding: 8px 12px;
            font-size: 14px;
            color: #333333;
        }
        
        QLineEdit#passwordEdit:focus, QLineEdit#confirmPasswordEdit:focus {
            border-color: #409eff;
            outline: none;
        }
        
        QPushButton#saveButton {
            background-color: #409eff;
            color: #ffffff;
            border: none;
            border-radius: 0px;
            font-size: 14px;
            font-weight: 500;
        }
        
        QPushButton#saveButton:hover {
            background-color: #66b1ff;
        }
        
        QPushButton#saveButton:pressed {
            background-color: #3a8ee6;
        }
        
        QPushButton#saveButton:disabled {
            background-color: #c0c4cc;
            color: #ffffff;
        }
        
        QPushButton#cancelButton {
            background-color: #ffffff;
            color: #666666;
            border: 1px solid #d0d0d0;
            border-radius: 0px;
            font-size: 14px;
            font-weight: 500;
        }
        
        QPushButton#cancelButton:hover {
            border-color: #b0b0b0;
            background-color: #f8f8f8;
        }
        
        QPushButton#cancelButton:pressed {
            background-color: #f0f0f0;
        }
    )");
}

void ChangePasswordDialog::onSaveClicked()
{
    if (validatePasswords()) {
        emit passwordChanged(passwordEdit->text());
        accept();
    }
}

void ChangePasswordDialog::onCancelClicked()
{
    reject();
}

void ChangePasswordDialog::onPasswordTextChanged()
{
    updateSaveButtonState();
}

void ChangePasswordDialog::onConfirmPasswordTextChanged()
{
    updateSaveButtonState();
}

bool ChangePasswordDialog::validatePasswords()
{
    QString password = passwordEdit->text();
    QString confirmPassword = confirmPasswordEdit->text();
    
    if (password.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入新密码");
        passwordEdit->setFocus();
        return false;
    }
    
    if (password.length() < 6) {
        QMessageBox::warning(this, "提示", "密码长度不能少于6位");
        passwordEdit->setFocus();
        return false;
    }
    
    if (password != confirmPassword) {
        QMessageBox::warning(this, "提示", "两次输入的密码不一致");
        confirmPasswordEdit->setFocus();
        return false;
    }
    
    return true;
}

void ChangePasswordDialog::updateSaveButtonState()
{
    bool hasPassword = !passwordEdit->text().isEmpty();
    bool hasConfirmPassword = !confirmPasswordEdit->text().isEmpty();
    
    saveButton->setEnabled(hasPassword && hasConfirmPassword);
}

QString ChangePasswordDialog::getNewPassword() const
{
    return passwordEdit->text();
}

QString ChangePasswordDialog::getConfirmPassword() const
{
    return confirmPasswordEdit->text();
}

