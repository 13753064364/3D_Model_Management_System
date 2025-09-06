#ifndef CUSTOMERPROFILEPAGE_H
#define CUSTOMERPROFILEPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>

class CustomerProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit CustomerProfilePage(QWidget *parent = nullptr);

signals:
    void addCustomerRequested();
    void deleteCustomerRequested(int customerId);
    void editCustomerRequested(int customerId);
    void viewCustomerRequested(int customerId);

private slots:
    void onSearchClicked();
    void onAddClicked();
    void onDeleteClicked();
    void onPageChanged();
    void onPageSizeChanged();
    void onJumpToPage();
    void onTableItemClicked(int row, int column);

private:
    void setupUI();
    void applyStyles();
    void updateCustomerList();
    void updatePagination();
    QWidget* createSearchBar();
    QWidget* createTableHeader();
    QWidget* createPaginationBar();

private:
    // 搜索区域
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QPushButton *addBtn;
    QPushButton *deleteBtn;
    
    // 表格
    QTableWidget *customerTable;
    
    // 分页区域
    QLabel *pageInfoLabel;
    QPushButton *prevPageBtn;
    QPushButton *nextPageBtn;
    QComboBox *pageSizeCombo;
    QLineEdit *jumpToEdit;
    QPushButton *jumpBtn;
    
    // 数据
    int currentPage;
    int pageSize;
    int totalCount;
    int totalPages;
};

#endif // CUSTOMERPROFILEPAGE_H
