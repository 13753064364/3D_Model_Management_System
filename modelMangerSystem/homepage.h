#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QHBoxLayout;

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

private slots:
    void onSearchTextChanged(const QString &text);
    void onSearchReturnPressed();

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
};

#endif // HOMEPAGE_H


