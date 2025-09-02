#include "homepage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QApplication>
#include <QScreen>

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    applyStyles();
}

void HomePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget *headerWidget = new QWidget();
    headerWidget->setObjectName("headerWidget");
    headerWidget->setFixedHeight(60);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 10, 20, 10);

    QHBoxLayout *leftLayout = new QHBoxLayout();
    leftLayout->setSpacing(12);
    logoLabel = new QLabel();
    logoLabel->setObjectName("headerLogo");
    logoLabel->setFixedSize(40, 40);
    titleLabel = new QLabel("3D模型管理系统");
    titleLabel->setObjectName("headerTitle");
    leftLayout->addWidget(logoLabel);
    leftLayout->addWidget(titleLabel);

    QHBoxLayout *userLayout = new QHBoxLayout();
    userLayout->setSpacing(8);
    userInfoLabel = new QLabel("用户名");
    userInfoLabel->setObjectName("userInfo");
    userAvatarLabel = new QLabel();
    userAvatarLabel->setObjectName("userAvatar");
    userAvatarLabel->setFixedSize(32, 32);
    userLayout->addWidget(userInfoLabel);
    userLayout->addWidget(userAvatarLabel);

    headerLayout->addLayout(leftLayout);
    headerLayout->addStretch();
    headerLayout->addLayout(userLayout);

    QWidget *contentWidget = new QWidget();
    contentWidget->setObjectName("contentWidget");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(60, 40, 60, 40);
    contentLayout->setSpacing(40);
    
    contentLayout->addStretch(1);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(8);
    searchEdit = new QLineEdit();
    searchEdit->setObjectName("searchEdit");
    searchEdit->setPlaceholderText("请输入姓名或手机号");
    searchEdit->setMinimumHeight(40);
    
    searchBtn = new QPushButton();
    searchBtn->setObjectName("searchBtn");
    searchBtn->setFixedSize(40, 40);
    searchBtn->setIcon(QIcon(":/images/images/search_icon.png"));
    
    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchBtn);

    connect(searchEdit, &QLineEdit::textChanged, this, &HomePage::onSearchTextChanged);
    connect(searchEdit, &QLineEdit::returnPressed, this, &HomePage::onSearchReturnPressed);
    connect(searchBtn, &QPushButton::clicked, this, &HomePage::onSearchReturnPressed);

    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setContentsMargins(0, 0, 0, 0);
    
    cardsLayout->addStretch();

    createFunctionCard("添加顾客", ":/images/images/add_customer.png", &btnAddCustomer, cardsLayout);
    createFunctionCard("顾客档案", ":/images/images/customer_profile.png", &btnCustomerProfile, cardsLayout);
    createFunctionCard("3D模型查看", ":/images/images/model_view.png", &btnModelView, cardsLayout);
    createFunctionCard("拍摄影像", ":/images/images/photo_imaging.png", &btnPhotoImaging, cardsLayout);
    createFunctionCard("系统管理", ":/images/images/system_settings.png", &btnSystemSettings, cardsLayout);
    
    cardsLayout->addStretch();

    connect(btnAddCustomer, &QPushButton::clicked, this, &HomePage::addCustomerClicked);
    connect(btnCustomerProfile, &QPushButton::clicked, this, &HomePage::customerProfileClicked);
    connect(btnModelView, &QPushButton::clicked, this, &HomePage::modelViewClicked);
    connect(btnPhotoImaging, &QPushButton::clicked, this, &HomePage::photoImagingClicked);
    connect(btnSystemSettings, &QPushButton::clicked, this, &HomePage::systemSettingsClicked);

    contentLayout->addLayout(searchLayout);
    contentLayout->addStretch(2);
    contentLayout->addLayout(cardsLayout);
    contentLayout->addStretch(2);

    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(contentWidget);
}

void HomePage::applyStyles()
{
    setStyleSheet(R"(
        QWidget { 
            background-color: #f5f5f5; 
            font-family: "Microsoft YaHei", "Segoe UI", sans-serif; 
        }
        
        /* 顶部栏样式 */
        QWidget#headerWidget { 
            background: #ffffff; 
            border-bottom: 1px solid #e0e0e0; 
        }
        QLabel#headerLogo { 
            background: transparent; 
        }
        QLabel#headerTitle { 
            font-size: 20pt; 
            font-weight: bold; 
            color: #2c3e50; 
            background: transparent;
        }
        QLabel#userInfo { 
            color: #606266; 
            font-size: 16px; 
            background: transparent;
        }
        QLabel#userAvatar { 
            background: #409eff; 
            border-radius: 16px; 
        }
        
        /* 内容区域样式 */
        QWidget#contentWidget { 
            background: url(':/images/images/background.jpg'); 
            background-position: center; 
            background-repeat: no-repeat; 
            background-size: cover; 
        }
        
        /* 搜索区域样式 */
        QLineEdit#searchEdit { 
            background: #ffffff; 
            border: 1px solid #dcdfe6; 
            border-radius: 6px; 
            padding: 10px 12px; 
            font-size: 16px; 
        }
        QLineEdit#searchEdit:focus { 
            border-color: #409eff; 
        }
        QPushButton#searchBtn { 
            background: transparent; 
            border: none; 
            border-radius: 6px; 
        }
        QPushButton#searchBtn:hover { 
            background: rgba(64, 158, 255, 0.1); 
        }
        
        /* 功能卡片样式 */
        QPushButton#cardButton { 
            background: #ffffff; 
            border: 1px solid #e0e0e0; 
            border-radius: 12px; 
            padding: 0px; 
        }
        QPushButton#cardButton:hover { 
            border-color: #409eff; 
            box-shadow: 0 4px 12px rgba(64, 158, 255, 0.3); 
            transform: translateY(-2px);
        }
        QPushButton#cardButton:pressed { 
            background: #f8f9fa; 
            transform: translateY(0px);
        }
        QWidget#functionCard { 
            background: transparent; 
        }
        QLabel#cardIcon { 
            background: transparent; 
        }
        QLabel#cardTitle { 
            color: #333333; 
            font-size: 16px; 
            font-weight: 600; 
            background: transparent;
        }
    )");
}

void HomePage::setUserInfo(const QString &infoText)
{
    userInfoLabel->setText(infoText);
}

void HomePage::setHeaderLogo(const QString &logoPath)
{
    if (!logoPath.isEmpty()) {
        QPixmap logo(logoPath);
        if (!logo.isNull()) {
            logoLabel->setPixmap(logo.scaled(logoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void HomePage::setUserAvatar(const QString &avatarPath)
{
    if (!avatarPath.isEmpty()) {
        QPixmap avatar(avatarPath);
        if (!avatar.isNull()) {
            userAvatarLabel->setPixmap(avatar.scaled(userAvatarLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    } else {
        userAvatarLabel->setStyleSheet("background: #409eff; border-radius: 16px;");
    }
}

void HomePage::onSearchTextChanged(const QString &text)
{
    emit searchRequested(text);
}

void HomePage::onSearchReturnPressed()
{
    emit searchRequested(searchEdit->text());
}

void HomePage::createFunctionCard(const QString &title, const QString &iconPath, QPushButton **button, QHBoxLayout *layout)
{
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int screenWidth = screenGeometry.width();
    int cardWidth = (screenWidth * 0.8) / 5 - 20;
    cardWidth = cardWidth * 0.8;
    int cardHeight = cardWidth * 1.2;
    
    QWidget *cardWidget = new QWidget();
    cardWidget->setObjectName("functionCard");
    cardWidget->setFixedSize(cardWidth, cardHeight);
    
    QVBoxLayout *cardLayout = new QVBoxLayout(cardWidget);
    cardLayout->setContentsMargins(15, 15, 15, 15);
    cardLayout->setSpacing(12);
    
    QLabel *iconLabel = new QLabel();
    iconLabel->setObjectName("cardIcon");
    int iconSize = cardWidth * 0.6;
    iconLabel->setFixedSize(iconSize, iconSize);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setScaledContents(true);
    iconLabel->setStyleSheet("QLabel#cardIcon { margin: 0 auto; }");
    
    QPixmap icon(iconPath);
    if (!icon.isNull()) {
        iconLabel->setPixmap(icon.scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        iconLabel->setStyleSheet("background: #e0e0e0; border-radius: " + QString::number(iconSize/2) + "px;");
    }
    
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("cardTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    
    cardLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    cardLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    
    *button = new QPushButton();
    (*button)->setObjectName("cardButton");
    (*button)->setFixedSize(cardWidth, cardHeight);
    (*button)->setLayout(cardLayout);
    
    layout->addWidget(*button);
}


