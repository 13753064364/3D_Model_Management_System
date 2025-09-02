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

signals:
    void storeEntered(int storeId);

private:
    void setupUI();
    void applyStyles();

private:
    QLabel *titleLabel;
    QTableWidget *table;
};

#endif // STORESELECTIONPAGE_H


