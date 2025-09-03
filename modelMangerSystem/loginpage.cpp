#include "loginpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QGraphicsDropShadowEffect>
#include <QPixmap>
#include <QMessageBox>
#include <QDialog>
#include <QTextEdit>
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

    agreementCheck = new QCheckBox("我已阅读及同意");
    agreementCheck->setObjectName("agreementCheck");
    
    userAgreementLabel = new QLabel("<a href=\"user_agreement\" style=\"color: #409EFF; text-decoration: none;\">《用户协议》</a>");
    userAgreementLabel->setOpenExternalLinks(false);
    userAgreementLabel->setObjectName("userAgreementLabel");
    
    privacyAgreementLabel = new QLabel("<a href=\"privacy_agreement\" style=\"color: #409EFF; text-decoration: none;\">《隐私协议》</a>");
    privacyAgreementLabel->setOpenExternalLinks(false);
    privacyAgreementLabel->setObjectName("privacyAgreementLabel");
    
    connect(userAgreementLabel, &QLabel::linkActivated, this, &LoginPage::onUserAgreementClicked);
    connect(privacyAgreementLabel, &QLabel::linkActivated, this, &LoginPage::onPrivacyAgreementClicked);

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
    QHBoxLayout *agreementLayout = new QHBoxLayout();
    agreementLayout->addWidget(agreementCheck);
    agreementLayout->addWidget(userAgreementLabel);
    agreementLayout->addWidget(privacyAgreementLabel);
    agreementLayout->addStretch();
    
    rightLayout->addLayout(agreementLayout);
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
    if (!agreementCheck->isChecked()) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("提示"));
        msgBox.setText(tr("请先阅读并同意用户协议和隐私协议"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setStyleSheet(
            "QMessageBox {"
            "    background-color: white;"
            "    border: 1px solid #dcdfe6;"
            "    border-radius: 8px;"
            "    min-width: 300px;"
            "    min-height: 120px;"
            "}"
            "QMessageBox QLabel {"
            "    color: #606266;"
            "    font-size: 14px;"
            "    padding: 20px;"
            "    text-align: center;"
            "}"
            "QMessageBox QPushButton {"
            "    background-color: #409eff;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 4px;"
            "    padding: 8px 20px;"
            "    font-size: 14px;"
            "    width: 120px;"
            "    min-width: 120px;"
            "    max-width: 120px;"
            "    min-height: 32px;"
            "}"
            "QMessageBox QPushButton:hover {"
            "    background-color: #66b1ff;"
            "}"
            "QMessageBox QPushButton:pressed {"
            "    background-color: #3a8ee6;"
            "}"
            "QMessageBox QDialogButtonBox {"
            "    qproperty-centerButtons: true;"
            "}"
        );
        
        QAbstractButton *okButton = msgBox.button(QMessageBox::Ok);
        if (okButton) {
            okButton->setFixedSize(120, 32);
        }
        
        msgBox.exec();
        return;
    }
    emit loginRequested(userEdit->text(), pwdEdit->text(), rememberCheck->isChecked());
}

void LoginPage::onUserAgreementClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("用户协议"));
    dialog.setModal(true);
    dialog.resize(600, 500);
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QTextEdit *textEdit = new QTextEdit();
    textEdit->setReadOnly(true);
    textEdit->setHtml(
        "<h2>用户协议</h2>"
        "<h3>1. 服务条款</h3>"
        "<p>欢迎使用3D模型管理系统。本协议是您与本公司之间关于您使用本系统所订立的协议。</p>"
        "<h3>2. 用户责任</h3>"
        "<p>用户在使用本系统时，应当遵守相关法律法规，不得利用本系统从事违法违规活动。</p>"
        "<h3>3. 知识产权</h3>"
        "<p>本系统的所有内容，包括但不限于文字、图片、音频、视频、软件、程序、版面设计等均受法律保护。</p>"
        "<h3>4. 隐私保护</h3>"
        "<p>我们重视用户的隐私保护，具体内容请参见《隐私协议》。</p>"
        "<h3>5. 免责声明</h3>"
        "<p>用户因使用本系统而产生的任何损失，本公司不承担任何责任。</p>"
        "<h3>6. 协议修改</h3>"
        "<p>本公司有权随时修改本协议，修改后的协议将在系统内公布。</p>"
        "<h3>7. 联系方式</h3>"
        "<p>如有疑问，请联系客服：400-123-4567</p>"
    );
    
    layout->addWidget(textEdit);
    
    QPushButton *closeBtn = new QPushButton(tr("关闭"));
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(closeBtn);
    
    dialog.exec();
}

void LoginPage::onPrivacyAgreementClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("隐私协议"));
    dialog.setModal(true);
    dialog.resize(600, 500);
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QTextEdit *textEdit = new QTextEdit();
    textEdit->setReadOnly(true);
    textEdit->setHtml(
        "<h2>隐私协议</h2>"
        "<h3>1. 信息收集</h3>"
        "<p>我们可能收集以下信息：用户名、密码、邮箱地址、使用记录等。</p>"
        "<h3>2. 信息使用</h3>"
        "<p>收集的信息将用于：提供系统服务、改善用户体验、安全防护等。</p>"
        "<h3>3. 信息保护</h3>"
        "<p>我们采用行业标准的安全措施保护您的个人信息，防止未经授权的访问、使用或泄露。</p>"
        "<h3>4. 信息共享</h3>"
        "<p>除法律要求外，我们不会向第三方分享您的个人信息。</p>"
        "<h3>5. Cookie使用</h3>"
        "<p>我们使用Cookie来改善用户体验，您可以通过浏览器设置管理Cookie。</p>"
        "<h3>6. 数据保留</h3>"
        "<p>我们仅在必要期间保留您的个人信息，超出期限后将安全删除。</p>"
        "<h3>7. 用户权利</h3>"
        "<p>您有权访问、更正、删除您的个人信息，或要求我们停止处理您的信息。</p>"
        "<h3>8. 联系我们</h3>"
        "<p>如有隐私相关问题，请联系：privacy@company.com</p>"
    );
    
    layout->addWidget(textEdit);
    
    QPushButton *closeBtn = new QPushButton(tr("关闭"));
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(closeBtn);
    
    dialog.exec();
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
        agreementCheck->setText("我已阅读及同意");
        userAgreementLabel->setText("<a href=\"user_agreement\" style=\"color: #409EFF; text-decoration: none;\">《用户协议》</a>");
        privacyAgreementLabel->setText("<a href=\"privacy_agreement\" style=\"color: #409EFF; text-decoration: none;\">《隐私协议》</a>");
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

void LoginPage::setSavedAccount(const QString &username, const QString &password, bool remember)
{
    if (!username.isEmpty()) {
        userEdit->setText(username);
        qDebug() << "设置保存的用户名:" << username;
    }
    
    if (!password.isEmpty()) {
        pwdEdit->setText(password);
        qDebug() << "设置保存的密码";
    }
    
    rememberCheck->setChecked(remember);
    qDebug() << "设置记住账号状态:" << remember;
}

void LoginPage::setLoginStatus(const QString &status, const QString &color)
{
    bottomIdLabel->setText(status);
    

    QString style = QString("QLabel { color: %1; font-weight: bold; font-size: 14px; }").arg(color);
    bottomIdLabel->setStyleSheet(style);
    

    bottomIdLabel->update();
    
    qDebug() << "设置登录状态:" << status << "颜色:" << color;
    qDebug() << "应用的样式:" << style;
}


