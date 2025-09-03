#include "storeselectionpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QDebug>

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
        
        // 进入按钮
        QWidget *btnWidget = new QWidget();
        QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);
        QPushButton *enterBtn = new QPushButton("进入");
        enterBtn->setObjectName("enterBtn");
        enterBtn->setMinimumHeight(30);
        enterBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: transparent;"
            "    color: #67C23A;"
            "    border: none;"
            "    font-size: 14px;"
            "    font-weight: bold;"
            "    padding: 5px 10px;"
            "}"
            "QPushButton:hover {"
            "    background-color: transparent;"
            "    color: #85ce61;"
            "}"
            "QPushButton:pressed {"
            "    background-color: transparent;"
            "    color: #5daf34;"
            "}"
        );
        btnLayout->addWidget(enterBtn);
        btnLayout->setAlignment(Qt::AlignCenter);
        btnLayout->setContentsMargins(10, 5, 10, 5);
        table->setCellWidget(i, 2, btnWidget);
        

        connect(enterBtn, &QPushButton::clicked, this, [this, i]() {
            emit storeEntered(i + 1);
        });
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


