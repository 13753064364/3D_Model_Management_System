#include "storeselectionpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>

StoreSelectionPage::StoreSelectionPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    applyStyles();
}

void StoreSelectionPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    titleLabel = new QLabel("选择门店");
    titleLabel->setAlignment(Qt::AlignCenter);

    table = new QTableWidget(1, 3);
    table->setHorizontalHeaderLabels(QStringList() << "门店名称" << "位置" << "操作");
    table->setMinimumWidth(600);


    table->setRowCount(1);
    QTableWidgetItem *loadingItem = new QTableWidgetItem("正在加载门店信息...");
    loadingItem->setTextAlignment(Qt::AlignCenter);
    table->setItem(0, 0, loadingItem);
    table->setSpan(0, 0, 1, 3);

    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    table->setColumnWidth(2, 100);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // 连接表格点击事件
    connect(table, &QTableWidget::cellClicked, this, [this](int row, int column) {
        if (column == 2) { // 操作列
            QTableWidgetItem *item = table->item(row, column);
            if (item && item->data(Qt::UserRole).isValid()) {
                int storeIndex = item->data(Qt::UserRole).toInt();
                emit storeEntered(storeIndex + 1);
            }
        }
    });

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(table);

    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

}

void StoreSelectionPage::updateStoreList(const QStringList &storeNames, const QStringList &storeLocations)
{
    qDebug() << "更新门店列表，门店数量:" << storeNames.size();
    qDebug() << "门店名称:" << storeNames;
    qDebug() << "门店位置:" << storeLocations;
    

    table->setRowCount(0);
    

    int rowCount = storeNames.size();
    table->setRowCount(rowCount);
    qDebug() << "设置表格行数:" << rowCount;
    

    for (int i = 0; i < rowCount; ++i) {
        // 门店名称
        QTableWidgetItem *storeItem = new QTableWidgetItem(storeNames[i]);
        storeItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(i, 0, storeItem);
        
        // 门店位置
        QString location = (i < storeLocations.size()) ? storeLocations[i] : "未知地址";
        QTableWidgetItem *locationItem = new QTableWidgetItem(location);
        locationItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(i, 1, locationItem);
        
        // 操作列 - 可点击的绿色文字
        QTableWidgetItem *operationItem = new QTableWidgetItem("进入");
        operationItem->setTextAlignment(Qt::AlignCenter);
        operationItem->setData(Qt::UserRole, i); // 存储索引
        operationItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table->setItem(i, 2, operationItem);
    }

    if (rowCount == 0) {
        table->setRowCount(1);
        QTableWidgetItem *noDataItem = new QTableWidgetItem("暂无门店数据");
        noDataItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(0, 0, noDataItem);
        table->setSpan(0, 0, 1, 3);
    }
    
    qDebug() << "门店列表更新完成";
}

void StoreSelectionPage::applyStyles()
{
    setStyleSheet(R"(
            QWidget { background-color: #f5f5f5; font-family: "Microsoft YaHei", sans-serif; }
            QLabel { font-size: 24px; font-weight: bold; color: #2c3e50; padding: 10px; }
            QTableWidget { background-color: white; border: 1px solid #dcdfe6; border-radius: 4px; gridline-color: #ebeef5; font-size: 14px; }
            QHeaderView::section { background-color: #f5f7fa; color: #606266; font-weight: bold; height: 40px; border: none; }
            QTableWidget::item { padding: 10px; color: #606266; }
            QTableWidget::item[column="2"] { color: #67C23A; font-weight: bold; }
            QTableWidget::item[column="2"]:hover { color: #85ce61; }
            QPushButton#enterBtn { 
                background-color: transparent !important; 
                color: #67C23A !important; 
                border: none !important; 
                font-size: 14px !important; 
                font-weight: bold !important;
                text-decoration: none !important;
                padding: 5px 10px !important;
            }
            QPushButton#enterBtn:hover { 
                background-color: transparent !important;
                color: #85ce61 !important; 
            }
            QPushButton#enterBtn:pressed { 
                background-color: transparent !important;
                color: #5daf34 !important; 
            }
            QPushButton#enterBtn:focus { 
                background-color: transparent !important;
                color: #67C23A !important; 
            }
        )");
}

bool StoreSelectionPage::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            QLabel *label = qobject_cast<QLabel*>(obj);
            if (label && label->property("storeIndex").isValid()) {
                int storeIndex = label->property("storeIndex").toInt();
                emit storeEntered(storeIndex + 1);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}


