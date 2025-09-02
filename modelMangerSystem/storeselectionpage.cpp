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

    table = new QTableWidget(3, 3);
    table->setHorizontalHeaderLabels(QStringList() << "门店名称" << "位置" << "操作");
    table->setMinimumWidth(600);

    QStringList stores = {"门店1", "门店2", "门店3"};
    QStringList locations = {"航海路", "五一广场", "市民中心"};

    for (int i = 0; i < 3; ++i) {
        QTableWidgetItem *storeItem = new QTableWidgetItem(stores[i]);
        storeItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(i, 0, storeItem);

        QTableWidgetItem *locationItem = new QTableWidgetItem(locations[i]);
        locationItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(i, 1, locationItem);

        QWidget *btnWidget = new QWidget();
        QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);
        QPushButton *enterBtn = new QPushButton("进入");
        enterBtn->setObjectName("enterBtn");
        enterBtn->setFixedWidth(60);
        btnLayout->addWidget(enterBtn);
        btnLayout->setAlignment(Qt::AlignCenter);
        btnLayout->setContentsMargins(0, 0, 0, 0);
        table->setCellWidget(i, 2, btnWidget);

        connect(enterBtn, &QPushButton::clicked, this, [this, i]() {
            emit storeEntered(i + 1);
        });
    }

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

void StoreSelectionPage::applyStyles()
{
    setStyleSheet(R"(
            QWidget { background-color: #f5f5f5; font-family: "Microsoft YaHei", sans-serif; }
            QLabel { font-size: 24px; font-weight: bold; color: #2c3e50; padding: 10px; }
            QTableWidget { background-color: white; border: 1px solid #dcdfe6; border-radius: 4px; gridline-color: #ebeef5; font-size: 14px; }
            QHeaderView::section { background-color: #f5f7fa; color: #606266; font-weight: bold; height: 40px; border: none; }
            QTableWidget::item { padding: 10px; color: #606266; }
            QPushButton#enterBtn { 
                background: none; 
                color: #409eff; 
                border: none; 
                font-size: 13px; 
                text-decoration: underline;
            }
            QPushButton#enterBtn:hover { 
                color: #66b1ff; 
            }
            QPushButton#enterBtn:pressed { 
                color: #3a8ee6; 
            }
        )");
}


