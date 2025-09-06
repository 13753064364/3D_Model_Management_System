#ifndef PHOTOIMAGINGPAGE_H
#define PHOTOIMAGINGPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

class PhotoImagingPage : public QWidget
{
    Q_OBJECT

public:
    explicit PhotoImagingPage(QWidget *parent = nullptr);

signals:
    void searchCustomerRequested(const QString &keyword);
    void startCaptureRequested();
    void stopCaptureRequested();

private slots:
    void onSearchClicked();
    void onCaptureClicked();
    void onSearchTextChanged();

private:
    void setupUI();
    void applyStyles();
    QWidget* createLeftPanel();
    QWidget* createCameraPreviewArea();
    QWidget* createSearchSection();
    QWidget* createTipItem(const QString &icon, const QString &text);

private:
    // 左侧面板
    QLabel *avatarLabel;
    QLabel *tip1Label;
    QLabel *tip2Label;
    QLabel *tip3Label;
    
    // 摄像头预览区域
    QLabel *cameraPreviewLabel;
    QPushButton *captureButton;
    
    // 搜索区域
    QComboBox *searchEdit;
    QPushButton *searchButton;
    
    // 状态
    bool isCapturing;
};

#endif // PHOTOIMAGINGPAGE_H
