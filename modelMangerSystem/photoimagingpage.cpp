#include "photoimagingpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QPainter>
#include <QBrush>
#include <QFrame>
#include <QDebug>

PhotoImagingPage::PhotoImagingPage(QWidget *parent)
    : QWidget(parent)
    , avatarLabel(nullptr)
    , tip1Label(nullptr)
    , tip2Label(nullptr)
    , tip3Label(nullptr)
    , cameraPreviewLabel(nullptr)
    , captureButton(nullptr)
    , searchEdit(nullptr)
    , searchButton(nullptr)
    , isCapturing(false)
{
    setupUI();
    applyStyles();
}

void PhotoImagingPage::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧面板
    QWidget *leftPanel = createLeftPanel();
    leftPanel->setFixedWidth(400);
    mainLayout->addWidget(leftPanel);

    // 中间分隔线
    QFrame *separator = new QFrame();
    separator->setObjectName("separator");
    separator->setFixedWidth(1);
    mainLayout->addWidget(separator);

    // 右侧区域（摄像头预览 + 搜索）
    QWidget *rightArea = new QWidget();
    rightArea->setObjectName("rightArea");
    QVBoxLayout *rightLayout = new QVBoxLayout(rightArea);
    rightLayout->setContentsMargins(40, 40, 40, 40);
    rightLayout->setSpacing(15);

    // 摄像头预览区域
    QWidget *cameraArea = createCameraPreviewArea();
    rightLayout->addWidget(cameraArea, 1);

    // 搜索区域
    QWidget *searchArea = createSearchSection();
    rightLayout->addWidget(searchArea, 0);

    mainLayout->addWidget(rightArea, 1);
}

QWidget* PhotoImagingPage::createLeftPanel()
{
    QWidget *leftPanel = new QWidget();
    leftPanel->setObjectName("leftPanel");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(80, 50, 10, 50);
    leftLayout->setSpacing(25);

    // 卡通头像
    avatarLabel = new QLabel();
    avatarLabel->setObjectName("avatarLabel");
    
    // 尝试加载头像图片，如果失败则使用默认图标
    QPixmap avatarPixmap(":/images/images/avatar.png");
    if (avatarPixmap.isNull()) {
        // 如果图片加载失败，创建一个默认的圆形头像
        avatarPixmap = QPixmap(250, 250);
        avatarPixmap.fill(Qt::transparent);
        QPainter painter(&avatarPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // 绘制圆形背景
        painter.setBrush(QBrush(QColor(74, 144, 226)));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(0, 0, 250, 250);
        
        // 绘制用户图标
        painter.setPen(QPen(Qt::white, 8));
        painter.setFont(QFont("Arial", 80, QFont::Bold));
        painter.drawText(0, 0, 250, 250, Qt::AlignCenter, "👤");
    }
    
    avatarLabel->setPixmap(avatarPixmap.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setFixedSize(300, 300);
    leftLayout->addWidget(avatarLabel);

    // 添加弹性空间
    leftLayout->addStretch();

    // 拍摄提示
    QLabel *tipsTitle = new QLabel("拍摄提示");
    tipsTitle->setObjectName("tipsTitle");
    tipsTitle->setAlignment(Qt::AlignCenter);
    tipsTitle->setStyleSheet("font-size: 36px; font-weight: 900; color: #2c3e50; letter-spacing: 3px;");
    leftLayout->addWidget(tipsTitle);

    // 提示项1
    QWidget *tip1 = createTipItem(":/images/images/no_headphone.png", "请勿带耳机、头饰");
    leftLayout->addWidget(tip1);

    // 提示项2
    QWidget *tip2 = createTipItem(":/images/images/no_face_cover.png", "请勿遮挡面部");
    leftLayout->addWidget(tip2);

    // 提示项3
    QWidget *tip3 = createTipItem(":/images/images/close_eyes.png", "请闭眼拍摄");
    leftLayout->addWidget(tip3);

    // 底部弹性空间
    leftLayout->addStretch();

    return leftPanel;
}

QWidget* PhotoImagingPage::createTipItem(const QString &iconPath, const QString &text)
{
    QWidget *tipItem = new QWidget();
    tipItem->setObjectName("tipItem");
    QHBoxLayout *tipLayout = new QHBoxLayout(tipItem);
    tipLayout->setContentsMargins(20, 16, 20, 16);
    tipLayout->setSpacing(16);

    // 图标
    QLabel *iconLabel = new QLabel();
    iconLabel->setObjectName("tipIcon");
    QPixmap iconPixmap(iconPath);
    if (!iconPixmap.isNull()) {
        iconLabel->setPixmap(iconPixmap.scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // 如果图片加载失败，显示默认图标
        iconLabel->setText("⚠️");
        iconLabel->setStyleSheet("font-size: 72px;");
    }
    iconLabel->setFixedSize(120, 120);
    iconLabel->setAlignment(Qt::AlignCenter);
    tipLayout->addWidget(iconLabel);

    // 文字
    QLabel *textLabel = new QLabel(text);
    textLabel->setObjectName("tipText");
    textLabel->setWordWrap(true);
    tipLayout->addWidget(textLabel);

    return tipItem;
}

QWidget* PhotoImagingPage::createCameraPreviewArea()
{
    QWidget *cameraArea = new QWidget();
    cameraArea->setObjectName("cameraArea");
    QVBoxLayout *cameraLayout = new QVBoxLayout(cameraArea);
    cameraLayout->setContentsMargins(0, 0, 0, 0);
    cameraLayout->setSpacing(20);

    // 摄像头预览区域
    cameraPreviewLabel = new QLabel();
    cameraPreviewLabel->setObjectName("cameraPreview");
    cameraPreviewLabel->setText("摄像头预览区域\n点击下方按钮开始拍摄");
    cameraPreviewLabel->setAlignment(Qt::AlignCenter);
    cameraPreviewLabel->setMinimumHeight(200);
    cameraLayout->addWidget(cameraPreviewLabel, 1);

    // 镜头按钮
    captureButton = new QPushButton("📷 镜头");
    captureButton->setObjectName("captureButton");
    captureButton->setFixedHeight(50);
    captureButton->setFixedWidth(120);
    cameraLayout->addWidget(captureButton, 0, Qt::AlignCenter);

    // 连接信号
    connect(captureButton, &QPushButton::clicked, this, &PhotoImagingPage::onCaptureClicked);

    return cameraArea;
}

QWidget* PhotoImagingPage::createSearchSection()
{
    QWidget *searchArea = new QWidget();
    searchArea->setObjectName("searchArea");
    QHBoxLayout *searchLayout = new QHBoxLayout(searchArea);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(0);

    // 创建搜索框容器
    QWidget *searchContainer = new QWidget();
    searchContainer->setObjectName("searchContainer");
    searchContainer->setFixedHeight(50);
    searchContainer->setFixedWidth(350);
    
    QHBoxLayout *containerLayout = new QHBoxLayout(searchContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    // 搜索输入框（使用QComboBox支持输入和选择）
    searchEdit = new QComboBox();
    searchEdit->setObjectName("searchEdit");
    searchEdit->setEditable(true);
    searchEdit->setInsertPolicy(QComboBox::NoInsert);
    searchEdit->setFixedHeight(50);
    searchEdit->setFixedWidth(300);
    
    // 设置占位符文本（通过添加一个占位符项）
    searchEdit->addItem("请输入顾客姓名或手机号选择顾客");
    searchEdit->setCurrentIndex(0);
    
    // 添加假数据
    searchEdit->addItem("张三 - 13800138001");
    searchEdit->addItem("李四 - 13800138002");
    searchEdit->addItem("王五 - 13800138003");
    searchEdit->addItem("赵六 - 13800138004");
    searchEdit->addItem("钱七 - 13800138005");
    
    containerLayout->addWidget(searchEdit);

    // 搜索按钮（内嵌到搜索框右侧）
    searchButton = new QPushButton("🔍");
    searchButton->setObjectName("searchButton");
    searchButton->setFixedSize(50, 50);
    containerLayout->addWidget(searchButton);

    searchLayout->addWidget(searchContainer, 0, Qt::AlignCenter);

    // 连接信号
    connect(searchButton, &QPushButton::clicked, this, &PhotoImagingPage::onSearchClicked);
    connect(searchEdit, QOverload<const QString &>::of(&QComboBox::currentTextChanged), this, &PhotoImagingPage::onSearchTextChanged);
    connect(searchEdit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PhotoImagingPage::onSearchClicked);

    return searchArea;
}

void PhotoImagingPage::applyStyles()
{
    setStyleSheet(R"(
        QWidget#leftPanel {
            background: #f8f9fa;
            border-right: 1px solid #e9ecef;
        }
        
        QWidget#rightArea {
            background: #ffffff;
        }
        
        QFrame#separator {
            background: #e9ecef;
        }
        
        QLabel#avatarLabel {
            background: #ffffff;
            border: 5px solid #dee2e6;
            border-radius: 150px;
            padding: 15px;
            margin-right: 20px;
        }
        
        QLabel#tipsTitle {
            font-size: 96px !important;
            font-weight: 900 !important;
            color: #495057 !important;
            margin: 20px 0 !important;
            letter-spacing: 3px !important;
            text-align: center !important;
        }
        
        /* 更具体的选择器 */
        QWidget#leftPanel QLabel#tipsTitle {
            font-size: 96px !important;
            font-weight: 900 !important;
            color: #2c3e50 !important;
        }
        
        QWidget#tipItem {
            background: #ffffff;
            border: 3px solid #dee2e6;
            border-radius: 20px;
            margin: 6px 20px 6px 0;
            min-height: 120px;
        }
        
        QWidget#tipItem:hover {
            border-color: #007bff;
        }
        
        QLabel#tipIcon {
            background: #f8f9fa;
            border-radius: 60px;
            padding: 12px;
        }
        
        QLabel#tipText {
            font-size: 80px;
            color: #495057;
            font-weight: 800;
            line-height: 1.4;
        }
        
        QWidget#cameraArea {
            background: #ffffff;
            border: 3px solid #007bff;
            border-radius: 12px;
            padding: 20px;
            margin: 20px;
        }
        
        QLabel#cameraPreview {
            background: #f8f9fa;
            border: 2px solid #007bff;
            border-radius: 8px;
            color: #6c757d;
            font-size: 16px;
            font-weight: 500;
            min-height: 200px;
        }
        
        QPushButton#captureButton {
            background: linear-gradient(135deg, #ffc107, #ff8f00);
            border: none;
            border-radius: 25px;
            color: #ffffff;
            font-size: 16px;
            font-weight: 600;
            padding: 12px 24px;
        }
        
        QPushButton#captureButton:hover {
            background: linear-gradient(135deg, #ff8f00, #ff6f00);
        }
        
        QPushButton#captureButton:pressed {
            background: linear-gradient(135deg, #ff6f00, #e65100);
        }
        
        QPushButton#captureButton:checked {
            background: linear-gradient(135deg, #dc3545, #c82333);
        }
        
        QWidget#searchArea {
            background: #f8f9fa;
            border: 2px solid #dee2e6;
            border-radius: 12px;
            padding: 15px;
            margin-top: 0px;
            min-height: 80px;
        }
        
        QWidget#searchContainer {
            background: #ffffff;
            border: 2px solid #ced4da;
            border-radius: 8px;
        }
        
        QWidget#searchContainer:focus-within {
            border-color: #007bff;
        }
        
        QComboBox#searchEdit {
            border: none !important;
            border-radius: 6px 0px 0px 6px !important;
            padding: 8px 12px !important;
            font-size: 16px !important;
            background: transparent !important;
            font-weight: 500 !important;
        }
        
        QComboBox#searchEdit:focus {
            border: none !important;
            outline: none !important;
        }
        
        QComboBox#searchEdit::drop-down {
            border: none !important;
            width: 20px !important;
        }
        
        QComboBox#searchEdit::down-arrow {
            image: none !important;
            border-left: 5px solid transparent !important;
            border-right: 5px solid transparent !important;
            border-top: 5px solid #6c757d !important;
            margin-right: 5px !important;
        }
        
        QPushButton#searchButton {
            background: #007bff;
            border: none;
            border-radius: 0px 6px 6px 0px;
            color: #ffffff;
            font-size: 20px;
            font-weight: 600;
            padding: 8px;
        }
        
        QPushButton#searchButton:hover {
            background: #0056b3;
        }
        
        QPushButton#searchButton:pressed {
            background: #004085;
        }
    )");
}

void PhotoImagingPage::onSearchClicked()
{
    QString keyword = searchEdit->currentText().trimmed();
    if (!keyword.isEmpty()) {
        qDebug() << "搜索顾客:" << keyword;
        emit searchCustomerRequested(keyword);
    }
}

void PhotoImagingPage::onCaptureClicked()
{
    if (!isCapturing) {
        // 开始拍摄
        isCapturing = true;
        captureButton->setText("⏹️ 停止");
        captureButton->setChecked(true);
        cameraPreviewLabel->setText("正在拍摄中...\n请保持面部在镜头中央");
        emit startCaptureRequested();
        qDebug() << "开始拍摄";
    } else {
        // 停止拍摄
        isCapturing = false;
        captureButton->setText("📷 镜头");
        captureButton->setChecked(false);
        cameraPreviewLabel->setText("摄像头预览区域\n点击下方按钮开始拍摄");
        emit stopCaptureRequested();
        qDebug() << "停止拍摄";
    }
}

void PhotoImagingPage::onSearchTextChanged()
{
    // 可以在这里添加实时搜索逻辑
    QString text = searchEdit->currentText();
    if (text.length() >= 2) {
        // 可以触发自动搜索
        qDebug() << "输入文本变化:" << text;
    }
}
