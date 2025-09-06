#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include "widget.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QHBoxLayout;
class QTabWidget;
class QWidget;
class QComboBox;
class QTextEdit;
class QVBoxLayout;
class QGridLayout;
class AddCustomerPage;
class CustomerProfilePage;

class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);

    void setUserInfo(const QString &infoText);
    void setHeaderLogo(const QString &logoPath);
    void setUserAvatar(const QString &avatarPath);

signals:
    void searchRequested(const QString &keyword);
    void addCustomerClicked();
    void customerProfileClicked();
    void modelViewClicked();
    void photoImagingClicked();
    void systemSettingsClicked();
    void tabClosed(int index);

private slots:
    void onSearchTextChanged(const QString &text);
    void onSearchReturnPressed();
    void onCardClicked();
    void onTabCloseRequested(int index);
    void onTabChanged(int index);

private:
    void setupUI();
    void applyStyles();
    void createFunctionCard(const QString &title, const QString &iconPath, QPushButton **button, QHBoxLayout *layout);

private:
    QLabel *logoLabel;
    QLabel *titleLabel;
    QLabel *userInfoLabel;
    QLabel *userAvatarLabel;
    
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    
    QPushButton *btnAddCustomer;
    QPushButton *btnCustomerProfile;
    QPushButton *btnModelView;
    QPushButton *btnPhotoImaging;
    QPushButton *btnSystemSettings;
    
    QWidget *searchWidget;
    QWidget *cardsWidget;
    QTabWidget *tabWidget;
    
    AddCustomerPage *addCustomerPage;
    CustomerProfilePage *customerProfilePage;
    QWidget *modelViewPage;
    Widget *photoImagingPage;
    QWidget *systemSettingsPage;
};

#endif // HOMEPAGE_H


