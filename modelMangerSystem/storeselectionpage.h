#ifndef STORESELECTIONPAGE_H
#define STORESELECTIONPAGE_H

#include <QWidget>

class QLabel;
class QTableWidget;

class StoreSelectionPage : public QWidget
{
    Q_OBJECT
public:
    explicit StoreSelectionPage(QWidget *parent = nullptr);
    
    // 更新门店列表
    void updateStoreList(const QStringList &storeNames, const QStringList &storeLocations);

signals:
    void storeEntered(int storeId);

private:
    void setupUI();
    void applyStyles();
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QLabel *titleLabel;
    QTableWidget *table;
};

#endif // STORESELECTIONPAGE_H


