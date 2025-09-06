#include "customerprofilepage.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDebug>

CustomerProfilePage::CustomerProfilePage(QWidget *parent)
    : QWidget(parent)
    , currentPage(1)
    , pageSize(10)
    , totalCount(100)
    , totalPages(10)
{
    setupUI();
    applyStyles();
    updateCustomerList();
    updatePagination();
}

void CustomerProfilePage::setupUI()
{
    setStyleSheet("background: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%);");
    
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
    cardLayout->setContentsMargins(30, 30, 30, 30);
    cardLayout->setSpacing(24);

    QWidget *titleWidget = new QWidget();
    QVBoxLayout *titleLayout = new QVBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(8);
    
    QLabel *titleLabel = new QLabel("顾客档案管理");
    titleLabel->setObjectName("pageTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QLabel *subtitleLabel = new QLabel("管理顾客信息，查看检测记录");
    subtitleLabel->setObjectName("pageSubtitle");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subtitleLabel);
    cardLayout->addWidget(titleWidget);

    cardLayout->addWidget(createSearchBar());

    cardLayout->addWidget(createTableHeader());

    cardLayout->addWidget(createPaginationBar());
    
    containerLayout->addWidget(cardContainer);
    mainLayout->addWidget(mainContainer);
}

QWidget* CustomerProfilePage::createSearchBar()
{
    QWidget *searchWidget = new QWidget();
    searchWidget->setObjectName("searchWidget");
    QHBoxLayout *searchLayout = new QHBoxLayout(searchWidget);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(16);

    QWidget *searchContainer = new QWidget();
    searchContainer->setObjectName("searchContainer");
    QHBoxLayout *searchContainerLayout = new QHBoxLayout(searchContainer);
    searchContainerLayout->setContentsMargins(0, 0, 0, 0);
    searchContainerLayout->setSpacing(0);
    
    searchEdit = new QLineEdit();
    searchEdit->setObjectName("searchEdit");
    searchEdit->setPlaceholderText("请输入姓名或手机号");
    
    searchBtn = new QPushButton("🔍");
    searchBtn->setObjectName("searchBtn");
    searchBtn->setFixedSize(50, 50);
    
    searchContainerLayout->addWidget(searchEdit);
    searchContainerLayout->addWidget(searchBtn);

    addBtn = new QPushButton("➕ 新增");
    addBtn->setObjectName("addBtn");
    
    deleteBtn = new QPushButton("🗑️ 删除");
    deleteBtn->setObjectName("deleteBtn");

    connect(searchBtn, &QPushButton::clicked, this, &CustomerProfilePage::onSearchClicked);
    connect(addBtn, &QPushButton::clicked, this, &CustomerProfilePage::onAddClicked);
    connect(deleteBtn, &QPushButton::clicked, this, &CustomerProfilePage::onDeleteClicked);
    
    searchLayout->addWidget(searchContainer);
    searchLayout->addStretch();
    searchLayout->addWidget(addBtn);
    searchLayout->addWidget(deleteBtn);
    
    return searchWidget;
}

QWidget* CustomerProfilePage::createTableHeader()
{
    QWidget *tableWidget = new QWidget();
    QVBoxLayout *tableLayout = new QVBoxLayout(tableWidget);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(0);

    customerTable = new QTableWidget();
    customerTable->setObjectName("customerTable");
    customerTable->setColumnCount(8);
    
    QStringList headers;
    headers << "" << "姓名" << "性别" << "年龄" << "手机号" << "问题项" << "上次检测时间" << "操作";
    customerTable->setHorizontalHeaderLabels(headers);

    customerTable->setAlternatingRowColors(true);
    customerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    customerTable->setSelectionMode(QAbstractItemView::SingleSelection);
    customerTable->setSortingEnabled(true);
    customerTable->horizontalHeader()->setStretchLastSection(true);
    customerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    customerTable->verticalHeader()->setVisible(false);

    customerTable->setColumnWidth(0, 80);   // 头像列
    customerTable->setColumnWidth(1, 120);  // 姓名列
    customerTable->setColumnWidth(2, 80);   // 性别列
    customerTable->setColumnWidth(3, 80);   // 年龄列
    customerTable->setColumnWidth(4, 150);  // 电话列
    customerTable->setColumnWidth(5, 200);  // 问题列
    customerTable->setColumnWidth(6, 120);  // 最后测试列
    customerTable->setColumnWidth(7, 180);  // 操作列

    customerTable->verticalHeader()->setDefaultSectionSize(60);

    connect(customerTable, &QTableWidget::cellClicked, this, &CustomerProfilePage::onTableItemClicked);
    
    tableLayout->addWidget(customerTable);
    
    return tableWidget;
}

QWidget* CustomerProfilePage::createPaginationBar()
{
    QWidget *paginationWidget = new QWidget();
    paginationWidget->setObjectName("paginationWidget");
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setContentsMargins(0, 0, 0, 0);
    paginationLayout->setSpacing(16);
    

    pageInfoLabel = new QLabel();
    pageInfoLabel->setObjectName("pageInfoLabel");
    

    prevPageBtn = new QPushButton("◀ 上一页");
    prevPageBtn->setObjectName("pageBtn");
    
    nextPageBtn = new QPushButton("下一页 ▶");
    nextPageBtn->setObjectName("pageBtn");
    

    QLabel *pageSizeLabel = new QLabel("条/页:");
    pageSizeLabel->setObjectName("paginationLabel");
    
    pageSizeCombo = new QComboBox();
    pageSizeCombo->setObjectName("pageSizeCombo");
    pageSizeCombo->addItems(QStringList() << "6" << "10" << "20" << "50");
    pageSizeCombo->setCurrentText("10");
    

    QLabel *jumpLabel = new QLabel("跳至:");
    jumpLabel->setObjectName("paginationLabel");
    
    jumpToEdit = new QLineEdit();
    jumpToEdit->setObjectName("jumpToEdit");
    jumpToEdit->setPlaceholderText("页码");
    jumpToEdit->setFixedWidth(80);
    
    jumpBtn = new QPushButton("跳转");
    jumpBtn->setObjectName("jumpBtn");

    connect(prevPageBtn, &QPushButton::clicked, this, &CustomerProfilePage::onPageChanged);
    connect(nextPageBtn, &QPushButton::clicked, this, &CustomerProfilePage::onPageChanged);
    connect(pageSizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CustomerProfilePage::onPageSizeChanged);
    connect(jumpBtn, &QPushButton::clicked, this, &CustomerProfilePage::onJumpToPage);
    
    paginationLayout->addWidget(pageInfoLabel);
    paginationLayout->addStretch();
    paginationLayout->addWidget(prevPageBtn);
    paginationLayout->addWidget(nextPageBtn);
    paginationLayout->addWidget(pageSizeLabel);
    paginationLayout->addWidget(pageSizeCombo);
    paginationLayout->addWidget(jumpLabel);
    paginationLayout->addWidget(jumpToEdit);
    paginationLayout->addWidget(jumpBtn);
    
    return paginationWidget;
}

void CustomerProfilePage::applyStyles()
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
            font-size: 32px;
            font-weight: 700;
            color: #2d3748;
            letter-spacing: -0.5px;
        }
        
        QLabel#pageSubtitle {
            font-size: 18px;
            font-weight: 400;
            color: #718096;
        }
        
        /* 搜索栏样式 */
        QWidget#searchWidget {
            background: transparent;
        }
        
        QWidget#searchContainer {
            background: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            min-width: 300px;
        }
        QWidget#searchContainer:focus-within {
            border-color: #667eea;
            box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
        }
        
        QLineEdit#searchEdit {
            background: transparent;
            border: none;
            padding: 16px 20px;
            font-size: 18px;
            color: #2d3748;
        }
        QLineEdit#searchEdit:focus {
            outline: none;
        }
        
        QPushButton#searchBtn {
            background: transparent;
            border: none;
            border-left: 1px solid #e2e8f0;
            font-size: 18px;
            color: #a0aec0;
        }
        QPushButton#searchBtn:hover {
            background: #f7fafc;
            color: #667eea;
        }
        
        /* 操作按钮 */
        QPushButton#addBtn {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                stop:0 #48bb78, stop:1 #38a169);
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 16px 24px;
            font-size: 16px;
            font-weight: 600;
            min-width: 100px;
        }
        QPushButton#addBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                stop:0 #38a169, stop:1 #2f855a);
        }
        
        QPushButton#deleteBtn {
            background: #e53e3e;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 16px 24px;
            font-size: 16px;
            font-weight: 600;
            min-width: 100px;
        }
        QPushButton#deleteBtn:hover {
            background: #c53030;
        }
        
        /* 表格样式 */
        QTableWidget#customerTable {
            background: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            gridline-color: #e2e8f0;
            selection-background-color: #667eea;
            selection-color: #ffffff;
            font-size: 18px;
        }
        
        QTableWidget#customerTable::item {
            padding: 20px 16px;
            border-bottom: 1px solid #f1f5f9;
            font-size: 18px;
            text-align: center;
            vertical-align: middle;
        }
        
        QTableWidget#customerTable::item[column="7"] {
            color: #409eff;
            text-decoration: underline;
            font-size: 18px;
        }
        
        QTableWidget#customerTable::item:selected {
            background: #667eea;
            color: #ffffff;
            border: none;
            outline: none;
        }
        
        QTableWidget#customerTable::item:selected[column="7"] {
            color: #ffffff;
            text-decoration: underline;
        }
        
        /* 确保选中时所有列的文字都显示为白色 */
        QTableWidget#customerTable::item:selected {
            color: #ffffff !important;
            background: #667eea !important;
        }
        
        /* 操作列选中时保持白色和下划线 */
        QTableWidget#customerTable::item:selected[column="7"] {
            color: #ffffff !important;
            text-decoration: underline;
            background: #667eea !important;
        }
        
        /* 确保所有表格项在选中时都显示白色文字 */
        QTableWidget#customerTable::item:selected * {
            color: #ffffff !important;
        }
        
        /* 确保操作列始终显示蓝色 */
        QTableWidget#customerTable::item[column="7"]:hover {
            color: #409eff;
            text-decoration: underline;
        }
        
        /* 移除选中时的虚线边框 */
        QTableWidget#customerTable {
            outline: none;
        }
        
        QTableWidget#customerTable::item:focus {
            outline: none;
            border: none;
        }
        
        QTableWidget#customerTable::item:hover {
            background: #f7fafc;
        }
        
        QHeaderView::section {
            background: #f8fafc;
            color: #4a5568;
            font-weight: 600;
            font-size: 18px;
            padding: 20px 16px;
            border: none;
            border-bottom: 2px solid #e2e8f0;
            border-right: 1px solid #e2e8f0;
        }
        
        QHeaderView::section:first {
            border-top-left-radius: 8px;
        }
        
        QHeaderView::section:last {
            border-top-right-radius: 8px;
            border-right: none;
        }
        
        /* 分页样式 */
        QWidget#paginationWidget {
            background: #f8fafc;
            border-radius: 8px;
            padding: 16px;
        }
        
        QLabel#pageInfoLabel {
            font-size: 16px;
            color: #4a5568;
            font-weight: 500;
        }
        
        QPushButton#pageBtn {
            background: #ffffff;
            color: #4a5568;
            border: 1px solid #e2e8f0;
            border-radius: 6px;
            padding: 12px 20px;
            font-size: 16px;
            font-weight: 500;
        }
        QPushButton#pageBtn:hover {
            background: #f7fafc;
            border-color: #cbd5e0;
        }
        QPushButton#pageBtn:pressed {
            background: #edf2f7;
        }
        
        QLabel#paginationLabel {
            font-size: 16px;
            color: #4a5568;
            font-weight: 500;
        }
        
        QComboBox#pageSizeCombo {
            background: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 6px;
            padding: 12px 16px;
            font-size: 16px;
            min-width: 80px;
        }
        QComboBox#pageSizeCombo:focus {
            border-color: #667eea;
        }
        
        QLineEdit#jumpToEdit {
            background: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 6px;
            padding: 12px 16px;
            font-size: 16px;
            text-align: center;
        }
        QLineEdit#jumpToEdit:focus {
            border-color: #667eea;
            outline: none;
        }
        
        QPushButton#jumpBtn {
            background: #667eea;
            color: #ffffff;
            border: none;
            border-radius: 6px;
            padding: 12px 20px;
            font-size: 16px;
            font-weight: 500;
        }
        QPushButton#jumpBtn:hover {
            background: #5a67d8;
        }
    )");
}

void CustomerProfilePage::updateCustomerList()
{
    customerTable->setRowCount(10);
    
    QStringList names = {"张三", "李四", "王五", "赵六", "钱七", "孙八", "周九", "吴十", "郑十一", "王十二"};
    QStringList genders = {"男", "女", "男", "女", "男", "女", "男", "女", "男", "女"};
    QStringList ages = {"25", "30", "28", "35", "22", "40", "26", "32", "29", "27"};
    QStringList phones = {"138****1234", "139****5678", "136****9012", "137****3456", "135****7890", 
                         "138****2345", "139****6789", "136****0123", "137****4567", "135****8901"};
    QStringList issues = {"皮肤干燥", "毛孔粗大", "色斑", "痘痘", "敏感", "暗沉", "细纹", "黑头", "油性", "混合性"};
    QStringList lastTests = {"2024-01-15", "2024-01-14", "2024-01-13", "2024-01-12", "2024-01-11",
                           "2024-01-10", "2024-01-09", "2024-01-08", "2024-01-07", "2024-01-06"};
    
    for (int i = 0; i < 10; ++i) {
        // 头像列
        QTableWidgetItem *avatarItem = new QTableWidgetItem("👤");
        avatarItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        avatarItem->setFlags(avatarItem->flags() & ~Qt::ItemIsEditable);
        customerTable->setItem(i, 0, avatarItem);
        
        // 姓名
        QTableWidgetItem *nameItem = new QTableWidgetItem(names[i]);
        nameItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        customerTable->setItem(i, 1, nameItem);
        
        // 性别
        QTableWidgetItem *genderItem = new QTableWidgetItem(genders[i]);
        genderItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        genderItem->setFlags(genderItem->flags() & ~Qt::ItemIsEditable);
        customerTable->setItem(i, 2, genderItem);
        
        // 年龄
        QTableWidgetItem *ageItem = new QTableWidgetItem(ages[i]);
        ageItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        ageItem->setFlags(ageItem->flags() & ~Qt::ItemIsEditable);
        customerTable->setItem(i, 3, ageItem);
        
        // 手机号
        QTableWidgetItem *phoneItem = new QTableWidgetItem(phones[i]);
        phoneItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        phoneItem->setFlags(phoneItem->flags() & ~Qt::ItemIsEditable);
        customerTable->setItem(i, 4, phoneItem);
        
        // 问题项
        QTableWidgetItem *issueItem = new QTableWidgetItem(issues[i]);
        issueItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        issueItem->setFlags(issueItem->flags() & ~Qt::ItemIsEditable);
        customerTable->setItem(i, 5, issueItem);
        
        // 上次检测时间
        QTableWidgetItem *testItem = new QTableWidgetItem(lastTests[i]);
        testItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        testItem->setFlags(testItem->flags() & ~Qt::ItemIsEditable);
        customerTable->setItem(i, 6, testItem);
        
        // 操作列
        QTableWidgetItem *actionItem = new QTableWidgetItem("详情  开始拍摄");
        actionItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        actionItem->setFlags(actionItem->flags() & ~Qt::ItemIsEditable);
        actionItem->setData(Qt::UserRole, i + 1); // 存储客户ID
        customerTable->setItem(i, 7, actionItem);
    }
}

void CustomerProfilePage::updatePagination()
{
    int start = (currentPage - 1) * pageSize + 1;
    int end = qMin(currentPage * pageSize, totalCount);
    
    pageInfoLabel->setText(QString("第 %1 - %2 条 / 共计 %3 条").arg(start).arg(end).arg(totalCount));
    
    prevPageBtn->setEnabled(currentPage > 1);
    nextPageBtn->setEnabled(currentPage < totalPages);
}

void CustomerProfilePage::onSearchClicked()
{
    QString keyword = searchEdit->text().trimmed();
    qDebug() << "搜索关键词:" << keyword;

}

void CustomerProfilePage::onAddClicked()
{
    emit addCustomerRequested();
}

void CustomerProfilePage::onDeleteClicked()
{
    int currentRow = customerTable->currentRow();
    if (currentRow >= 0) {
        QTableWidgetItem *item = customerTable->item(currentRow, 7);
        if (item) {
            int customerId = item->data(Qt::UserRole).toInt();
            emit deleteCustomerRequested(customerId);
        }
    } else {
        QMessageBox::information(this, "提示", "请先选择要删除的顾客");
    }
}

void CustomerProfilePage::onPageChanged()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn == prevPageBtn && currentPage > 1) {
        currentPage--;
    } else if (btn == nextPageBtn && currentPage < totalPages) {
        currentPage++;
    }
    
    updatePagination();
    updateCustomerList();
}

void CustomerProfilePage::onPageSizeChanged()
{
    pageSize = pageSizeCombo->currentText().toInt();
    totalPages = (totalCount + pageSize - 1) / pageSize;
    currentPage = 1;
    
    updatePagination();
    updateCustomerList();
}

void CustomerProfilePage::onJumpToPage()
{
    int targetPage = jumpToEdit->text().toInt();
    if (targetPage >= 1 && targetPage <= totalPages) {
        currentPage = targetPage;
        updatePagination();
        updateCustomerList();
    } else {
        QMessageBox::warning(this, "警告", QString("页码必须在 1 到 %1 之间").arg(totalPages));
    }
}

void CustomerProfilePage::onTableItemClicked(int row, int column)
{
    if (column == 7) {
        QTableWidgetItem *item = customerTable->item(row, column);
        if (item) {
            int customerId = item->data(Qt::UserRole).toInt();
            

            QPoint clickPos = customerTable->mapFromGlobal(QCursor::pos());
            QRect itemRect = customerTable->visualItemRect(item);
            int relativeX = clickPos.x() - itemRect.x();
            int itemWidth = itemRect.width();
            
            // "详情  开始拍摄" 中，"详情"大约占前1/3，"开始拍摄"占后2/3
            if (relativeX < itemWidth * 0.4) {
                // "详情"
                emit viewCustomerRequested(customerId);
            } else {
                // "开始拍摄"
                emit startShootingRequested(customerId);
            }
        }
    }
}
