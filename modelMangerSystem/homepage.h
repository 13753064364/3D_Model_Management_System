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
class QMenu;
class QAction;

class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);

    void setUserInfo(const QString &infoText);
    void setHeaderLogo(const QString &logoPath);
    void setUserAvatar(const QString &avatarPath);
    CustomerProfilePage* getCustomerProfilePage() const;

signals:
    void searchRequested(const QString &keyword);
    void addCustomerClicked();
    void customerProfileClicked();
    void modelViewClicked();
    void photoImagingClicked();
    void systemSettingsClicked();
    void tabClosed(int index);
    void userInfoClicked();
    void changePasswordClicked();
    void logoutClicked();

private slots:
    void onSearchTextChanged(const QString &text);
    void onSearchReturnPressed();
    void onCardClicked();
    void onTabCloseRequested(int index);
    void onTabChanged(int index);
    void onAvatarClicked();
    void onUserInfoAction();
    void onChangePasswordAction();
    void onSystemSettingsAction();
    void onLogoutAction();

private:
    void setupUI();
    void applyStyles();
    void createFunctionCard(const QString &title, const QString &iconPath, QPushButton **button, QHBoxLayout *layout);
    void setupAvatarMenu();
    bool eventFilter(QObject *watched, QEvent *event) override;

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
    
    // 用户头像下拉菜单相关
    QMenu *avatarMenu;
    QAction *userInfoAction;
    QAction *changePasswordAction;
    QAction *systemSettingsAction;
    QAction *logoutAction;
};

#endif // HOMEPAGE_H


