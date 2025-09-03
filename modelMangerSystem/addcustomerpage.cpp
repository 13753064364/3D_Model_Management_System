#include "addcustomerpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>

AddCustomerPage::AddCustomerPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    applyStyles();
}

void AddCustomerPage::setupUI()
{
    setStyleSheet("background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget *mainContainer = new QWidget();
    mainContainer->setObjectName("mainContainer");
    QVBoxLayout *containerLayout = new QVBoxLayout(mainContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    QWidget *topBar = new QWidget();
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(4);
    containerLayout->addWidget(topBar);

    QWidget *cardContainer = new QWidget();
    cardContainer->setObjectName("cardContainer");
    QVBoxLayout *cardLayout = new QVBoxLayout(cardContainer);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    cardLayout->setSpacing(30);

    QWidget *titleWidget = new QWidget();
    QVBoxLayout *titleLayout = new QVBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(8);
    
    QLabel *titleLabel = new QLabel("添加顾客信息");
    titleLabel->setObjectName("pageTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QLabel *subtitleLabel = new QLabel("请填写顾客的基本信息");
    subtitleLabel->setObjectName("pageSubtitle");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subtitleLabel);
    cardLayout->addWidget(titleWidget);

    QWidget *formWidget = new QWidget();
    formWidget->setObjectName("formWidget");
    QVBoxLayout *formLayout = new QVBoxLayout(formWidget);
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->setSpacing(24);

    QWidget *row1 = new QWidget();
    QHBoxLayout *row1Layout = new QHBoxLayout(row1);
    row1Layout->setContentsMargins(0, 0, 0, 0);
    row1Layout->setSpacing(20);
    
    QWidget *nameGroup = createInputGroup("姓名", "请输入顾客姓名", &nameEdit, "text");
    QWidget *genderGroup = createSelectGroup("性别", "请选择性别", &genderCombo, QStringList() << "男" << "女");
    
    row1Layout->addWidget(nameGroup);
    row1Layout->addWidget(genderGroup);
    formLayout->addWidget(row1);

    QWidget *row2 = new QWidget();
    QHBoxLayout *row2Layout = new QHBoxLayout(row2);
    row2Layout->setContentsMargins(0, 0, 0, 0);
    row2Layout->setSpacing(20);
    
    QWidget *ageGroup = createInputGroup("年龄", "请输入年龄", &ageEdit, "number");
    QWidget *phoneGroup = createInputGroup("手机号", "请输入手机号码", &phoneEdit, "tel");
    
    row2Layout->addWidget(ageGroup);
    row2Layout->addWidget(phoneGroup);
    formLayout->addWidget(row2);

    QWidget *row3 = new QWidget();
    QHBoxLayout *row3Layout = new QHBoxLayout(row3);
    row3Layout->setContentsMargins(0, 0, 0, 0);
    row3Layout->setSpacing(20);
    
    QWidget *skinGroup = createSelectGroup("肤质", "请选择肤质类型", &skinTypeCombo, 
                                          QStringList() << "干性" << "油性" << "混合性" << "敏感性" << "中性");
    
    row3Layout->addWidget(skinGroup);
    row3Layout->addStretch();
    formLayout->addWidget(row3);

    QWidget *row4 = new QWidget();
    QVBoxLayout *row4Layout = new QVBoxLayout(row4);
    row4Layout->setContentsMargins(0, 0, 0, 0);
    row4Layout->setSpacing(8);
    
    QLabel *remarkLabel = new QLabel("备注");
    remarkLabel->setObjectName("fieldLabel");
    remarkEdit = new QTextEdit();
    remarkEdit->setObjectName("formTextEdit");
    remarkEdit->setPlaceholderText("请输入备注信息...");
    remarkEdit->setMaximumHeight(80);
    
    row4Layout->addWidget(remarkLabel);
    row4Layout->addWidget(remarkEdit);
    formLayout->addWidget(row4);
    
    cardLayout->addWidget(formWidget);

    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 20, 0, 0);
    buttonLayout->setSpacing(16);
    
    saveBtn = new QPushButton("💾 保存");
    saveBtn->setObjectName("primaryBtn");
    startPhotoBtn = new QPushButton("📷 开始拍摄");
    startPhotoBtn->setObjectName("successBtn");
    cancelBtn = new QPushButton("❌ 取消");
    cancelBtn->setObjectName("defaultBtn");

    connect(saveBtn, &QPushButton::clicked, this, &AddCustomerPage::onSaveClicked);
    connect(startPhotoBtn, &QPushButton::clicked, this, &AddCustomerPage::onStartPhotoClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &AddCustomerPage::onCancelClicked);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveBtn);
    buttonLayout->addWidget(startPhotoBtn);
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addStretch();
    
    cardLayout->addWidget(buttonWidget);
    containerLayout->addWidget(cardContainer);
    mainLayout->addWidget(mainContainer);
}

void AddCustomerPage::applyStyles()
{
    setStyleSheet(R"(
        QWidget { 
            font-family: "Segoe UI", "Microsoft YaHei", -apple-system, BlinkMacSystemFont, sans-serif; 
        }
        
        /* 主容器 */
        QWidget#mainContainer {
            background: transparent;
        }
        
        /* 顶部装饰条 */
        QWidget#topBar {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, 
                stop:0 #667eea, stop:0.5 #764ba2, stop:1 #667eea);
            border-radius: 2px;
        }
        
        /* 卡片容器 */
        QWidget#cardContainer {
            background: rgba(255, 255, 255, 0.95);
            border-radius: 16px;
            margin: 20px;
            box-shadow: 0 20px 40px rgba(0, 0, 0, 0.1);
        }
        
        /* 标题样式 */
        QLabel#pageTitle {
            font-size: 28px;
            font-weight: 700;
            color: #2d3748;
            letter-spacing: -0.5px;
        }
        
        QLabel#pageSubtitle {
            font-size: 16px;
            font-weight: 400;
            color: #718096;
        }
        
        /* 字段标签 */
        QLabel#fieldLabel {
            font-size: 14px;
            font-weight: 600;
            color: #4a5568;
            margin-bottom: 4px;
        }
        
        /* 输入框样式 */
        QLineEdit#formInput {
            background: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 12px 16px;
            font-size: 16px;
            color: #2d3748;
            min-height: 20px;
        }
        QLineEdit#formInput:focus {
            border-color: #667eea;
            outline: none;
            box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
        }
        QLineEdit#formInput:hover {
            border-color: #cbd5e0;
        }
        
        /* 下拉框样式 */
        QComboBox#formCombo {
            background: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 12px 16px;
            font-size: 16px;
            color: #2d3748;
            min-height: 20px;
        }
        QComboBox#formCombo:focus {
            border-color: #667eea;
            box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
        }
        QComboBox#formCombo:hover {
            border-color: #cbd5e0;
        }
        QComboBox#formCombo::drop-down {
            border: none;
            width: 30px;
            background: transparent;
        }
        QComboBox#formCombo::down-arrow {
            image: none;
            border-left: 6px solid transparent;
            border-right: 6px solid transparent;
            border-top: 6px solid #a0aec0;
            margin-right: 8px;
        }
        QComboBox QAbstractItemView {
            background: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            selection-background-color: #667eea;
            selection-color: #ffffff;
            padding: 4px;
        }
        
        /* 多行文本框 */
        QTextEdit#formTextEdit {
            background: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 12px 16px;
            font-size: 16px;
            color: #2d3748;
        }
        QTextEdit#formTextEdit:focus {
            border-color: #667eea;
            outline: none;
            box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
        }
        QTextEdit#formTextEdit:hover {
            border-color: #cbd5e0;
        }
        
        /* 按钮样式 */
        QPushButton#primaryBtn {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                stop:0 #667eea, stop:1 #764ba2);
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 14px 28px;
            font-size: 16px;
            font-weight: 600;
            min-width: 120px;
            min-height: 20px;
        }
        QPushButton#primaryBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                stop:0 #5a67d8, stop:1 #6b46c1);
            transform: translateY(-1px);
        }
        QPushButton#primaryBtn:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                stop:0 #4c51bf, stop:1 #553c9a);
            transform: translateY(0px);
        }
        
        QPushButton#successBtn {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                stop:0 #48bb78, stop:1 #38a169);
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 14px 28px;
            font-size: 16px;
            font-weight: 600;
            min-width: 120px;
            min-height: 20px;
        }
        QPushButton#successBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                stop:0 #38a169, stop:1 #2f855a);
            transform: translateY(-1px);
        }
        QPushButton#successBtn:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                stop:0 #2f855a, stop:1 #276749);
            transform: translateY(0px);
        }
        
        QPushButton#defaultBtn {
            background: #e2e8f0;
            color: #4a5568;
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 14px 28px;
            font-size: 16px;
            font-weight: 600;
            min-width: 120px;
            min-height: 20px;
        }
        QPushButton#defaultBtn:hover {
            background: #cbd5e0;
            border-color: #cbd5e0;
            transform: translateY(-1px);
        }
        QPushButton#defaultBtn:pressed {
            background: #a0aec0;
            border-color: #a0aec0;
            transform: translateY(0px);
        }
    )");
}

void AddCustomerPage::onSaveClicked()
{
    emit saveRequested();
}

void AddCustomerPage::onStartPhotoClicked()
{
    emit startPhotoRequested();
}

void AddCustomerPage::onCancelClicked()
{
    emit cancelRequested();
}

QWidget* AddCustomerPage::createInputGroup(const QString &label, const QString &placeholder, QLineEdit **edit, const QString &type)
{
    QWidget *group = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(group);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    
    QLabel *labelWidget = new QLabel(label);
    labelWidget->setObjectName("fieldLabel");
    
    *edit = new QLineEdit();
    (*edit)->setObjectName("formInput");
    (*edit)->setPlaceholderText(placeholder);
    
    layout->addWidget(labelWidget);
    layout->addWidget(*edit);
    
    return group;
}

QWidget* AddCustomerPage::createSelectGroup(const QString &label, const QString &placeholder, QComboBox **combo, const QStringList &options)
{
    QWidget *group = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(group);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    
    QLabel *labelWidget = new QLabel(label);
    labelWidget->setObjectName("fieldLabel");
    
    *combo = new QComboBox();
    (*combo)->setObjectName("formCombo");
    (*combo)->addItem(placeholder);
    for (const QString &option : options) {
        (*combo)->addItem(option);
    }
    
    layout->addWidget(labelWidget);
    layout->addWidget(*combo);
    
    return group;
}
