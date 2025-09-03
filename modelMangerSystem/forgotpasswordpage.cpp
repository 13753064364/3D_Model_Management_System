#include "forgotpasswordpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

ForgotPasswordPage::ForgotPasswordPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    applyStyles();
}

void ForgotPasswordPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    titleLabel = new QLabel("3D模型管理系统");
    titleLabel->setAlignment(Qt::AlignCenter);

    loginTitle = new QLabel("用户登录");
    loginTitle->setAlignment(Qt::AlignCenter);

    forgotTitle = new QLabel("忘记密码");
    forgotTitle->setAlignment(Qt::AlignCenter);

    phoneLabel = new QLabel("手机号：");
    phoneEdit = new QLineEdit();
    phoneEdit->setPlaceholderText("请输入手机号");

    QHBoxLayout *codeLayout = new QHBoxLayout();
    codeLabel = new QLabel("验证码：");
    codeEdit = new QLineEdit();
    codeEdit->setPlaceholderText("请输入短信验证码");
    getCodeBtn = new QPushButton("获取验证码");
    getCodeBtn->setObjectName("getCodeBtn");
    getCodeBtn->setFixedWidth(100);

    codeLayout->addWidget(codeLabel);
    codeLayout->addWidget(codeEdit);
    codeLayout->addWidget(getCodeBtn);

    pwdLabel = new QLabel("密码：");
    pwdEdit = new QLineEdit();
    pwdEdit->setPlaceholderText("请输入密码");
    pwdEdit->setEchoMode(QLineEdit::Password);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    saveBtn = new QPushButton("保存");
    cancelBtn = new QPushButton("取消");
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);

    backLogin = new QLabel("<a href=\"#\" style=\"text-decoration: none;\">登录</a>");
    backLogin->setAlignment(Qt::AlignCenter);
    backLogin->setTextFormat(Qt::RichText);
    backLogin->setTextInteractionFlags(Qt::TextBrowserInteraction);
    backLogin->setOpenExternalLinks(false);
    connect(backLogin, &QLabel::linkActivated, this, &ForgotPasswordPage::onBackToLoginClicked);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(loginTitle);
    mainLayout->addWidget(forgotTitle);
    mainLayout->addWidget(phoneLabel);
    mainLayout->addWidget(phoneEdit);
    mainLayout->addLayout(codeLayout);
    mainLayout->addWidget(pwdLabel);
    mainLayout->addWidget(pwdEdit);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(backLogin);

    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 30, 30, 30);
}

void ForgotPasswordPage::applyStyles()
{
    setStyleSheet(R"(
            QWidget {
                background-color: #f5f5f5;
                font-family: "Microsoft YaHei", sans-serif;
            }
            QLabel { color: #333333; font-size: 14px; }
            QLabel[title="true"] { font-size: 24px; font-weight: bold; color: #2c3e50; padding: 10px; }
            QLabel[subtitle="true"] { font-size: 18px; font-weight: bold; color: #409eff; padding: 8px; }
            QLineEdit { border: 1px solid #dcdfe6; border-radius: 4px; padding: 10px; font-size: 14px; background-color: white; }
            QLineEdit:focus { border-color: #409eff; }
            QPushButton { background-color: #409eff; color: white; border: none; border-radius: 4px; padding: 10px; font-size: 14px; }
            QPushButton:hover { background-color: #66b1ff; }
            QPushButton:pressed { background-color: #3a8ee6; }
            QPushButton#getCodeBtn { background-color: #909399; font-size: 12px; padding: 8px; }
        )");

    titleLabel->setProperty("title", true);
    forgotTitle->setProperty("subtitle", true);
}

void ForgotPasswordPage::onBackToLoginClicked()
{
    emit backToLoginRequested();
}


