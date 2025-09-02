#include "loginpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QGraphicsDropShadowEffect>
#include <QPixmap>
#include <QResizeEvent>
#include <QDebug>

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    applyStyles();
}

void LoginPage::setupUI()
{
    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(40, 40, 40, 40);
    outer->addStretch();

    cardWidget = new QWidget(this);
    cardWidget->setObjectName("card");
    cardWidget->setFixedSize(900, 540);

    auto *shadow = new QGraphicsDropShadowEffect(cardWidget);
    shadow->setBlurRadius(10);
    shadow->setXOffset(0);
    shadow->setYOffset(2);
    shadow->setColor(QColor(0, 0, 0, 25));
    cardWidget->setGraphicsEffect(shadow);

    QHBoxLayout *splitLayout = new QHBoxLayout(cardWidget);
    splitLayout->setContentsMargins(0, 0, 0, 0);
    splitLayout->setSpacing(0);

    leftPane = new QWidget(cardWidget);
    leftPane->setObjectName("leftPane");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPane);
    leftLayout->setContentsMargins(24, 24, 24, 24);
    leftLayout->setSpacing(12);

    QHBoxLayout *leftTop = new QHBoxLayout();
    smallLogoLabel = new QLabel();
    smallLogoLabel->setObjectName("smallLogo");
    smallLogoLabel->setFixedSize(36, 36);
    systemTitleLabel = new QLabel("3D模型管理系统");
    systemTitleLabel->setObjectName("systemTitle");
    leftTop->addWidget(smallLogoLabel);
    leftTop->addSpacing(8);
    leftTop->addWidget(systemTitleLabel);
    leftTop->addStretch();

    bigLogoLabel = new QLabel();
    bigLogoLabel->setObjectName("bigLogo");
    bigLogoLabel->setMinimumHeight(300);
    bigLogoLabel->setAlignment(Qt::AlignCenter);

    leftLayout->addLayout(leftTop);
    leftLayout->addStretch();
    leftLayout->addWidget(bigLogoLabel);
    leftLayout->addStretch();

    rightPane = new QWidget(cardWidget);
    rightPane->setObjectName("rightPane");
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPane);
    rightLayout->setContentsMargins(28, 28, 28, 28);
    rightLayout->setSpacing(18);

    QHBoxLayout *langRow = new QHBoxLayout();
    langRow->addStretch();
    languageBtn = new QPushButton("简体中文");
    languageBtn->setObjectName("langBtn");
    langRow->addWidget(languageBtn);
    connect(languageBtn, &QPushButton::clicked, [this]() {
        isEnglish = !isEnglish;
        applyLanguageTexts();
    });

    titleLabel = new QLabel("3D模型管理系统");
    titleLabel->setAlignment(Qt::AlignCenter);
    loginTitle = new QLabel("用户登录");
    loginTitle->setAlignment(Qt::AlignCenter);

    userLabel = new QLabel("用户名");
    userEdit = new QLineEdit();
    pwdLabel = new QLabel("密码");
    pwdEdit = new QLineEdit();
    pwdEdit->setEchoMode(QLineEdit::Password);

    agreementCheck = new QCheckBox("我已阅读及同意《用户协议》《隐私协议》");
    agreementCheck->setObjectName("agreementCheck");

    QHBoxLayout *rowLayout = new QHBoxLayout();
    rememberCheck = new QCheckBox("记住账号");
    forgotLabel = new QLabel("<a href=\"#\" style=\"text-decoration: none;\">忘记密码</a>");
    forgotLabel->setAlignment(Qt::AlignRight);
    forgotLabel->setTextFormat(Qt::RichText);
    forgotLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    forgotLabel->setOpenExternalLinks(false);
    connect(forgotLabel, &QLabel::linkActivated, this, &LoginPage::onForgotPasswordClicked);
    rowLayout->addWidget(rememberCheck);
    rowLayout->addStretch();
    rowLayout->addWidget(forgotLabel);

    loginBtn = new QPushButton("登录");
    loginBtn->setMinimumHeight(48);
    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::onLoginClicked);

    bottomIdLabel = new QLabel("用户ID: 未登录");
    bottomIdLabel->setAlignment(Qt::AlignCenter);

    rightLayout->addLayout(langRow);
    rightLayout->addWidget(titleLabel);
    rightLayout->addWidget(loginTitle);
    rightLayout->addWidget(userLabel);
    rightLayout->addWidget(userEdit);
    rightLayout->addWidget(pwdLabel);
    rightLayout->addWidget(pwdEdit);
    rightLayout->addWidget(agreementCheck);
    rightLayout->addLayout(rowLayout);
    rightLayout->addWidget(loginBtn);
    rightLayout->addWidget(bottomIdLabel);

    splitLayout->addWidget(leftPane, 1);
    splitLayout->addWidget(rightPane, 1);

    QHBoxLayout *centerRow = new QHBoxLayout();
    centerRow->addStretch();
    centerRow->addWidget(cardWidget);
    centerRow->addStretch();
    outer->addLayout(centerRow);
    outer->addStretch();

    buildForgotDialog();
}

void LoginPage::applyStyles()
{
    QString baseStyle = R"(
        LoginPage {
            font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
        }
        QWidget#card { 
            background: #ffffff; 
            border-radius: 12px; 
        }
        QWidget#leftPane { 
            background: #f2f3f5; 
            border-top-left-radius: 12px; 
            border-bottom-left-radius: 12px; 
        }
        QWidget#rightPane { 
            background: #ffffff; 
            border-top-right-radius: 12px; 
            border-bottom-right-radius: 12px; 
        }
        QLabel { 
            color: #333333; 
            font-size: 14px; 
        }
        QLabel#systemTitle { 
            font-size: 16pt; 
            font-weight: 600; 
            color: #2c3e50; 
        }
        QLabel[role="title"] { 
            font-size: 22pt; 
            font-weight: bold; 
            color: #2c3e50; 
            padding: 6px 0; 
        }
        QLabel[role="subtitle"] { 
            font-size: 14pt; 
            color: #606266; 
            padding: 4px 0 12px; 
        }
        QLineEdit { 
            border: 1px solid #dcdfe6; 
            border-radius: 6px; 
            padding: 10px 12px; 
            background: #ffffff; 
        }
        QLineEdit:focus { 
            border-color: #409eff; 
        }
        QCheckBox { 
            color: #606266; 
            font-size: 12pt; 
        }
        QPushButton { 
            border: none; 
            border-radius: 6px; 
            color: #ffffff; 
            font-size: 14pt; 
            font-weight: 600; 
            height: 48px;
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #409eff, stop:1 #66b1ff); 
        }
        QPushButton:hover { 
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #66b1ff, stop:1 #85c1ff); 
        }
        QPushButton#langBtn { 
            background: transparent; 
            color: #409eff; 
            border-radius: 4px; 
            height: 28px; 
            padding: 0 8px; 
        }
        QPushButton#langBtn:hover { 
            background: rgba(64,158,255,0.08); 
        }
        QLabel[role="bottomInfo"] { 
            color: #909399; 
            font-size: 11pt; 
            padding-top: 8px; 
        }
        QLabel#smallLogo { 
            background: transparent; 
        }
        QLabel#bigLogo { 
            background: transparent; 
        }
        QWidget#overlay { 
            background: rgba(0,0,0,0.35); 
        }
        QWidget#forgotDialog { 
            background: #ffffff; 
            border-radius: 10px; 
        }
        QLabel#dialogTitle {
            font-size: 18pt;
            font-weight: bold;
            color: #2c3e50;
            padding: 8px 0;
        }
        QLineEdit::placeholder {
            color: #c0c4cc;
        }
        QPushButton#getCodeBtn {
            background: transparent;
            color: #409eff;
            border: none;
            text-decoration: underline;
            font-size: 12pt;
            padding: 4px 8px;
        }
        QPushButton#getCodeBtn:hover {
            color: #66b1ff;
        }
        QPushButton#cancelBtn {
            background: #ffffff;
            color: #333333;
            border: 1px solid #dcdfe6;
            border-radius: 4px;
            padding: 6px 12px;
            font-size: 13px;
            min-height: 28px;
        }
        QPushButton#cancelBtn:hover {
            border-color: #c0c4cc;
        }
        QPushButton#saveBtn {
            background: #409eff;
            color: #ffffff;
            border: none;
            border-radius: 4px;
            padding: 6px 12px;
            font-size: 13px;
            min-height: 28px;
        }
        QPushButton#saveBtn:hover {
            background: #66b1ff;
        }
    )";
    
    setStyleSheet(baseStyle);

    titleLabel->setProperty("role", "title");
    loginTitle->setProperty("role", "subtitle");
    bottomIdLabel->setProperty("role", "bottomInfo");
}

void LoginPage::onForgotPasswordClicked()
{
    overlayWidget->resize(size());
    overlayWidget->show();
    overlayWidget->raise();
}

void LoginPage::onLoginClicked()
{
    emit loginRequested(userEdit->text(), pwdEdit->text(), rememberCheck->isChecked());
}

void LoginPage::buildForgotDialog()
{
    overlayWidget = new QWidget(this);
    overlayWidget->setObjectName("overlay");
    overlayWidget->hide();
    overlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);

    QVBoxLayout *overlayLayout = new QVBoxLayout(overlayWidget);
    overlayLayout->setContentsMargins(0, 0, 0, 0);
    overlayLayout->addStretch();

    forgotDialog = new QWidget(overlayWidget);
    forgotDialog->setObjectName("forgotDialog");
    forgotDialog->setFixedSize(420, 360);
    forgotDialog->setStyleSheet("QWidget#forgotDialog { background: #ffffff; border-radius: 10px; }");
    auto *dialogShadow = new QGraphicsDropShadowEffect(forgotDialog);
    dialogShadow->setBlurRadius(12);
    dialogShadow->setXOffset(0);
    dialogShadow->setYOffset(2);
    dialogShadow->setColor(QColor(0,0,0,35));
    forgotDialog->setGraphicsEffect(dialogShadow);

    QVBoxLayout *dlg = new QVBoxLayout(forgotDialog);
    dlg->setContentsMargins(24, 24, 24, 24);
    dlg->setSpacing(14);

    QLabel *dlgTitle = new QLabel(tr("忘记密码"));
    dlgTitle->setAlignment(Qt::AlignCenter);
    dlgTitle->setObjectName("dialogTitle");
    
    QHBoxLayout *phoneRow = new QHBoxLayout();
    QLabel *phoneLbl = new QLabel(tr("手机号"));
    fpPhoneEdit = new QLineEdit();
    fpPhoneEdit->setPlaceholderText(tr("请输入手机号"));
    phoneRow->addWidget(phoneLbl);
    phoneRow->addWidget(fpPhoneEdit);

    QHBoxLayout *codeRow = new QHBoxLayout();
    QLabel *codeLbl = new QLabel(tr("验证码"));
    fpCodeEdit = new QLineEdit();
    fpCodeEdit->setPlaceholderText(tr("请输入短信验证码"));
    fpGetCodeBtn = new QPushButton(tr("获取验证码"));
    fpGetCodeBtn->setObjectName("getCodeBtn");
    codeRow->addWidget(codeLbl);
    codeRow->addWidget(fpCodeEdit);
    codeRow->addWidget(fpGetCodeBtn);

    QHBoxLayout *pwdRow = new QHBoxLayout();
    QLabel *pwdLbl = new QLabel(tr("新密码"));
    fpNewPwdEdit = new QLineEdit();
    fpNewPwdEdit->setEchoMode(QLineEdit::Password);
    fpNewPwdEdit->setPlaceholderText(tr("请输入新密码"));
    pwdRow->addWidget(pwdLbl);
    pwdRow->addWidget(fpNewPwdEdit);

    QHBoxLayout *btnRow = new QHBoxLayout();
    fpSaveBtn = new QPushButton(tr("保存"));
    fpSaveBtn->setObjectName("saveBtn");
    fpCancelBtn = new QPushButton(tr("取消"));
    fpCancelBtn->setObjectName("cancelBtn");
    btnRow->addWidget(fpSaveBtn);
    btnRow->addStretch();
    btnRow->addWidget(fpCancelBtn);

    dlg->addWidget(dlgTitle);
    dlg->addLayout(phoneRow);
    dlg->addLayout(codeRow);
    dlg->addLayout(pwdRow);
    dlg->addLayout(btnRow);

    QHBoxLayout *center = new QHBoxLayout();
    center->addStretch();
    center->addWidget(forgotDialog);
    center->addStretch();

    overlayLayout->addLayout(center);
    overlayLayout->addStretch();

    connect(fpCancelBtn, &QPushButton::clicked, overlayWidget, &QWidget::hide);
}

void LoginPage::applyLanguageTexts()
{
    if (isEnglish) {
        languageBtn->setText("English");
        titleLabel->setText("3D Model Management System");
        loginTitle->setText("User Login");
        userLabel->setText("Username");
        pwdLabel->setText("Password");
        agreementCheck->setText("I have read and agree to the User Agreement and Privacy Policy");
        rememberCheck->setText("Remember me");
        forgotLabel->setText("<a href=\"#\" style=\"text-decoration: none;\">Forgot password</a>");
        loginBtn->setText("Sign In");
        bottomIdLabel->setText("User ID: Not signed");
        systemTitleLabel->setText("3D Model Management System");
    } else {
        languageBtn->setText("简体中文");
        titleLabel->setText("3D模型管理系统");
        loginTitle->setText("用户登录");
        userLabel->setText("用户名");
        pwdLabel->setText("密码");
        agreementCheck->setText("我已阅读及同意《用户协议》《隐私协议》");
        rememberCheck->setText("记住账号");
        forgotLabel->setText("<a href=\"#\" style=\"text-decoration: none;\">忘记密码</a>");
        loginBtn->setText("登录");
        bottomIdLabel->setText("用户ID: 未登录");
        systemTitleLabel->setText("3D模型管理系统");
    }
}

void LoginPage::setBackgroundImage(const QString &imagePath)
{
    backgroundImagePath = imagePath;
    if (!backgroundImagePath.isEmpty()) {
        QPixmap testPixmap(backgroundImagePath);
        if (!testPixmap.isNull()) {
            QString backgroundStyle = QString(
                "LoginPage { background: url('%1'); background-position: center; background-repeat: no-repeat; background-size: cover; }"
            ).arg(backgroundImagePath);
            
            QString baseStyle = R"(
                LoginPage {
                    font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
                }
                QWidget#card { 
                    background: #ffffff; 
                    border-radius: 12px; 
                }
                QWidget#leftPane { 
                    background: #f2f3f5; 
                    border-top-left-radius: 12px; 
                    border-bottom-left-radius: 12px; 
                }
                QWidget#rightPane { 
                    background: #ffffff; 
                    border-top-right-radius: 12px; 
                    border-bottom-right-radius: 12px; 
                }
                QLabel { 
                    color: #333333; 
                    font-size: 14px; 
                }
                QLabel#systemTitle { 
                    font-size: 16pt; 
                    font-weight: 600; 
                    color: #2c3e50; 
                }
                QLabel[role="title"] { 
                    font-size: 22pt; 
                    font-weight: bold; 
                    color: #2c3e50; 
                    padding: 6px 0; 
                }
                QLabel[role="subtitle"] { 
                    font-size: 14pt; 
                    color: #606266; 
                    padding: 4px 0 12px; 
                }
                QLineEdit { 
                    border: 1px solid #dcdfe6; 
                    border-radius: 6px; 
                    padding: 10px 12px; 
                    background: #ffffff; 
                }
                QLineEdit:focus { 
                    border-color: #409eff; 
                }
                QCheckBox { 
                    color: #606266; 
                    font-size: 12pt; 
                }
                QPushButton { 
                    border: none; 
                    border-radius: 6px; 
                    color: #ffffff; 
                    font-size: 14pt; 
                    font-weight: 600; 
                    height: 48px;
                    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #409eff, stop:1 #66b1ff); 
                }
                QPushButton:hover { 
                    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #66b1ff, stop:1 #85c1ff); 
                }
                QPushButton#langBtn { 
                    background: transparent; 
                    color: #409eff; 
                    border-radius: 4px; 
                    height: 28px; 
                    padding: 0 8px; 
                }
                QPushButton#langBtn:hover { 
                    background: rgba(64,158,255,0.08); 
                }
                QLabel[role="bottomInfo"] { 
                    color: #909399; 
                    font-size: 11pt; 
                    padding-top: 8px; 
                }
                QLabel#smallLogo { 
                    background: transparent; 
                }
                QLabel#bigLogo { 
                    background: transparent; 
                }
                QWidget#overlay { 
                    background: rgba(0,0,0,0.35); 
                }
                QWidget#forgotDialog { 
                    background: #ffffff; 
                    border-radius: 10px; 
                }
                QLabel#dialogTitle {
                    font-size: 18pt;
                    font-weight: bold;
                    color: #2c3e50;
                    padding: 8px 0;
                }
                QLineEdit::placeholder {
                    color: #c0c4cc;
                }
                QPushButton#getCodeBtn {
                    background: transparent;
                    color: #409eff;
                    border: none;
                    text-decoration: underline;
                    font-size: 12pt;
                    padding: 4px 8px;
                }
                QPushButton#getCodeBtn:hover {
                    color: #66b1ff;
                }
                QPushButton#cancelBtn {
                    background: #ffffff;
                    color: #333333;
                    border: 1px solid #dcdfe6;
                    border-radius: 4px;
                    padding: 6px 12px;
                    font-size: 13px;
                    min-height: 28px;
                }
                QPushButton#cancelBtn:hover {
                    border-color: #c0c4cc;
                }
                QPushButton#saveBtn {
                    background: #409eff;
                    color: #ffffff;
                    border: none;
                    border-radius: 4px;
                    padding: 6px 12px;
                    font-size: 13px;
                    min-height: 28px;
                }
                QPushButton#saveBtn:hover {
                    background: #66b1ff;
                }
            )";
            
            setStyleSheet(backgroundStyle + "\n" + baseStyle);
        } else {
            qDebug() << "Background image not found:" << backgroundImagePath;
        }
    }
}

void LoginPage::setLogos(const QString &smallLogo, const QString &bigLogo)
{
    smallLogoPath = smallLogo;
    bigLogoPath = bigLogo;
    if (!smallLogoPath.isEmpty()) {
        QPixmap p(smallLogoPath);
        if (!p.isNull()) {
            smallLogoLabel->setPixmap(p.scaled(smallLogoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    if (!bigLogoPath.isEmpty()) {
        QPixmap p(bigLogoPath);
        if (!p.isNull()) {
            bigLogoLabel->setPixmap(p.scaled(420, 280, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void LoginPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (overlayWidget && overlayWidget->isVisible()) {
        overlayWidget->resize(size());
    }
}


