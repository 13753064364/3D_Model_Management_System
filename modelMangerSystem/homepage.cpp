#include "homepage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QApplication>
#include <QScreen>
#include <QTabWidget>
#include <QStackedWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QGridLayout>
#include "addcustomerpage.h"

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
    headerWidget->setFixedHeight(80);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(30, 15, 30, 15);

    QHBoxLayout *leftLayout = new QHBoxLayout();
    leftLayout->setSpacing(12);
    logoLabel = new QLabel();
    logoLabel->setObjectName("headerLogo");
    logoLabel->setFixedSize(50, 50);
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
    userAvatarLabel->setFixedSize(40, 40);
    userLayout->addWidget(userInfoLabel);
    userLayout->addWidget(userAvatarLabel);

    headerLayout->addLayout(leftLayout);
    headerLayout->addStretch();
    headerLayout->addLayout(userLayout);

    mainLayout->addWidget(headerWidget);

    // 创建背景容器
    QWidget *backgroundContainer = new QWidget();
    backgroundContainer->setObjectName("backgroundContainer");
    QVBoxLayout *backgroundLayout = new QVBoxLayout(backgroundContainer);
    backgroundLayout->setContentsMargins(0, 0, 0, 0);
    backgroundLayout->setSpacing(0);

    // 创建搜索区域
    searchWidget = new QWidget();
    searchWidget->setObjectName("searchWidget");
    searchWidget->setStyleSheet("background-color: transparent;");
    QVBoxLayout *searchContentLayout = new QVBoxLayout(searchWidget);
    searchContentLayout->setContentsMargins(60, 40, 60, 40);
    searchContentLayout->setSpacing(40);
    
    searchContentLayout->addStretch(1);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(0);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    
    // 创建搜索框容器
    QWidget *searchContainer = new QWidget();
    searchContainer->setObjectName("searchContainer");
    QHBoxLayout *containerLayout = new QHBoxLayout(searchContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);
    
    searchEdit = new QLineEdit();
    searchEdit->setObjectName("searchEdit");
    searchEdit->setPlaceholderText("请输入姓名或手机号");
    searchEdit->setMinimumHeight(50);
    
    searchBtn = new QPushButton();
    searchBtn->setObjectName("searchBtn");
    searchBtn->setFixedSize(50, 50);
    searchBtn->setIcon(QIcon(":/images/images/search_icon.png"));
    
    containerLayout->addWidget(searchEdit);
    containerLayout->addWidget(searchBtn);
    
    searchLayout->addWidget(searchContainer);

    connect(searchEdit, &QLineEdit::textChanged, this, &HomePage::onSearchTextChanged);
    connect(searchEdit, &QLineEdit::returnPressed, this, &HomePage::onSearchReturnPressed);
    connect(searchBtn, &QPushButton::clicked, this, &HomePage::onSearchReturnPressed);

    // 创建卡片区域
    cardsWidget = new QWidget();
    cardsWidget->setStyleSheet("background-color: transparent;");
    QVBoxLayout *cardsContentLayout = new QVBoxLayout(cardsWidget);
    cardsContentLayout->setContentsMargins(60, 0, 60, 40);
    cardsContentLayout->setSpacing(40);

    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setContentsMargins(0, 0, 0, 0);
    
    cardsLayout->addStretch();

    createFunctionCard("添加顾客", ":/images/images/add_customer.png", &btnAddCustomer, cardsLayout);
    createFunctionCard("顾客档案", ":/images/images/customer_profile.png", &btnCustomerProfile, cardsLayout);
    createFunctionCard("3D模型查看", ":/images/images/model_view.png", &btnModelView, cardsLayout);
    createFunctionCard("拍摄影像", ":/images/images/photo_imaging.png", &btnPhotoImaging, cardsLayout);
    createFunctionCard("系统管理", ":/images/images/system_settings.png", &btnSystemSettings, cardsLayout);
    
    cardsLayout->addStretch();

    connect(btnAddCustomer, &QPushButton::clicked, this, &HomePage::onCardClicked);
    connect(btnCustomerProfile, &QPushButton::clicked, this, &HomePage::onCardClicked);
    connect(btnModelView, &QPushButton::clicked, this, &HomePage::onCardClicked);
    connect(btnPhotoImaging, &QPushButton::clicked, this, &HomePage::onCardClicked);
    connect(btnSystemSettings, &QPushButton::clicked, this, &HomePage::onCardClicked);

    searchContentLayout->addLayout(searchLayout);
    searchContentLayout->addStretch(2);
    searchContentLayout->addLayout(cardsLayout);
    searchContentLayout->addStretch(2);

    tabWidget = new QTabWidget();
    tabWidget->setObjectName("tabWidget");
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setStyleSheet("QTabWidget::pane { background: transparent; border: none; }");

    addCustomerPage = new AddCustomerPage();
    customerProfilePage = new QWidget();
    customerProfilePage->setStyleSheet("background-color: #ffffff;");
    modelViewPage = new QWidget();
    modelViewPage->setStyleSheet("background-color: #ffffff;");
    photoImagingPage = new Widget();
    photoImagingPage->setStyleSheet("background-color: #ffffff;");
    systemSettingsPage = new QWidget();
    systemSettingsPage->setStyleSheet("background-color: #ffffff;");

    QWidget *homePage = new QWidget();
    homePage->setObjectName("homePage");
    homePage->setStyleSheet("background-color: transparent !important;");
    homePage->setAttribute(Qt::WA_OpaquePaintEvent, false);
    QVBoxLayout *homeLayout = new QVBoxLayout(homePage);
    homeLayout->setContentsMargins(0, 0, 0, 0);
    homeLayout->addWidget(searchWidget);
    homeLayout->addWidget(cardsWidget);
    tabWidget->addTab(homePage, "主页");

    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &HomePage::onTabCloseRequested);
    connect(tabWidget, &QTabWidget::currentChanged, this, &HomePage::onTabChanged);

    backgroundLayout->addWidget(tabWidget);
    mainLayout->addWidget(backgroundContainer);
}

void HomePage::applyStyles()
{
    setStyleSheet(R"(
        QWidget { 
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
            font-size: 24pt; 
            font-weight: bold; 
            color: #2c3e50; 
            background: transparent;
        }
        QLabel#userInfo { 
            color: #606266; 
            font-size: 18px; 
            background: transparent;
        }
        QLabel#userAvatar { 
            background: #409eff; 
            border-radius: 20px; 
        }
        
        /* 背景容器样式 */
        QWidget#backgroundContainer { 
            background: url(':/images/images/background.jpg'); 
            background-position: center; 
            background-repeat: no-repeat; 
            background-size: cover; 
        }
        
        /* 主页标签页透明 */
        QWidget#homePage {
            background: transparent !important;
        }
        
        /* 搜索区域样式 */
        QWidget#searchWidget {
            background: transparent;
        }
        
        /* 搜索容器样式 */
        QWidget#searchContainer {
            background: #ffffff;
            border: 1px solid #dcdfe6;
            border-radius: 6px;
        }
        QWidget#searchContainer:focus-within {
            border-color: #409eff;
        }
        
        /* 搜索框样式 */
        QLineEdit#searchEdit { 
            background: transparent; 
            border: none; 
            padding: 12px 15px; 
            font-size: 18px; 
        }
        QLineEdit#searchEdit:focus { 
            border: none;
            outline: none;
        }
        
        /* 搜索按钮样式 */
        QPushButton#searchBtn { 
            background: transparent; 
            border: none; 
            border-left: 1px solid #dcdfe6;
            border-radius: 0px;
            margin: 0px;
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
        
        /* 标签页样式 */
        QTabWidget#tabWidget {
            background: transparent;
            border: none;
        }
        QTabWidget::pane {
            border: none;
            background: transparent;
        }
        QTabWidget::pane[tabIndex="0"] {
            background: transparent !important;
        }
        QTabBar::tab {
            background: #f5f5f5;
            color: #606266;
            padding: 8px 16px;
            margin-right: 2px;
            border: 1px solid #e0e0e0;
            border-bottom: none;
            border-radius: 8px 8px 0px 0px;
            min-width: 80px;
        }
        QTabBar::tab:selected {
            background: #ffffff;
            color: #409eff;
            border-bottom: 1px solid #ffffff;
            border-radius: 8px 8px 0px 0px;
        }
        QTabBar::tab:hover {
            background: #ecf5ff;
            color: #409eff;
            border-radius: 8px 8px 0px 0px;
        }
        QTabBar::close-button {
            image: none;
            background: transparent;
            border: none;
            border-radius: 8px;
            width: 16px;
            height: 16px;
            margin-left: 4px;
            font-size: 12px;
            font-weight: bold;
            color: #909399;
        }
        QTabBar::close-button:hover {
            background: #f56c6c;
            color: #ffffff;
        }
        QTabBar::close-button::after {
            content: "×";
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

void HomePage::onCardClicked()
{
    QPushButton *sender = qobject_cast<QPushButton*>(QObject::sender());
    if (!sender) return;

    QString tabTitle;
    QWidget *page = nullptr;

    if (sender == btnAddCustomer) {
        tabTitle = "添加顾客";
        page = addCustomerPage;
        emit addCustomerClicked();
    } else if (sender == btnCustomerProfile) {
        tabTitle = "顾客档案";
        page = customerProfilePage;
        emit customerProfileClicked();
    } else if (sender == btnModelView) {
        tabTitle = "3D模型查看";
        page = modelViewPage;
        emit modelViewClicked();
    } else if (sender == btnPhotoImaging) {
        tabTitle = "拍摄影像";
        page = photoImagingPage;
        emit photoImagingClicked();
    } else if (sender == btnSystemSettings) {
        tabTitle = "系统管理";
        page = systemSettingsPage;
        emit systemSettingsClicked();
    }

    if (page && !tabTitle.isEmpty()) {
        for (int i = 0; i < tabWidget->count(); ++i) {
            if (tabWidget->tabText(i) == tabTitle) {
                tabWidget->setCurrentIndex(i);
                return;
            }
        }

        tabWidget->addTab(page, tabTitle);
        tabWidget->setCurrentWidget(page);
    }
}

void HomePage::onTabCloseRequested(int index)
{
    if (index > 0) {
        tabWidget->removeTab(index);
        emit tabClosed(index);
    }
}

void HomePage::onTabChanged(int index)
{

    if (index == 0) {
        QWidget *homePage = tabWidget->widget(0);
        if (homePage) {
            homePage->setStyleSheet("background-color: transparent !important;");
            homePage->setAttribute(Qt::WA_OpaquePaintEvent, false);
            homePage->update();
        }
    }
}




