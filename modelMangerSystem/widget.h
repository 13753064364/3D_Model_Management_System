#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>

#include "camerathread.h"
#include "drivers/camera.h"
#include "drivers/ledcontroller.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


    void InitUI();


private slots:
    void on_CloseDevice_clicked();
    void on_OpenDevice_clicked();
    void on_FindCameras_clicked();
    void on_comboBox_currentIndexChanged(int index);
    void on_IncreaseBrightness_clicked();
    void on_DecreaseBrightness_clicked();
    void on_SerialPortReadyRead();
//    void on_StartGrabbing_2_clicked();
//    void on_StopGrabbing_clicked();
    void onCheckBox20Clicked();
    void onCheckBox25Clicked();
    void onCheckBox35Clicked();


private slots:
    void on_Open_clicked();

    void on_Close_clicked();
    void on_Send_clicked();
//    void on_Clean_clicked();
    void on_SaveButton_clicked();
    void on_GetButton_clicked();
    void on_SetButton_clicked();
    void on_AutoBalanceWhiteButton_clicked();
//    void on_NaturalLightButton_clicked();
    void on_UVLightButton_clicked();
    void on_White1Button_clicked();
    void on_White2Button_clicked();
    void on_White3Button_clicked();
    void on_UV1Button_clicked();
    void on_UV2Button_clicked();
    void on_Backlight1Button_clicked();
    void on_Backlight2Button_clicked();
    void on_horizontalSlider_valueChanged(int value);
    void on_AllOffButton_clicked();
    void on_OneClickFillLightButton_clicked();
    void on_MultiModeButton_clicked(); 
    void onMultiModeTimerTimeout(); 
    void openSavedFolder(const QString& path); 

private:
    Ui::Widget *ui;
    Camera m_cameras[3];
    CameraThread* m_cameraThreads[3];
    QImage m_currentImages[3];
    QSerialPort *serilaPort;
    QImage m_latestImages[3];
    QTimer* m_updateTimers[3];
    QLabel* m_displayLabels[3];
    LedController* m_ledController;
    volatile bool m_imageBusy[3] = {false, false, false};
    int m_currentCameraIndex = -1; 
    MV_CC_DEVICE_INFO_LIST m_deviceList; 
    void EnsureSerialPortOpen();
    void autoOpenSerialPort();
    void hideLedTab(); 
    void startNextMode(); 
    void saveCurrentModeImages(); 
    LedController::LightGroup m_selectedLightGroup = LedController::LightGroup::White1; 
    int m_defaultWhiteBrightness = 0;
    
    QTimer* m_multiModeTimer = nullptr;
    int m_currentModeIndex = 0;
    QString m_multiModeSaveDir;
    bool m_isMultiModeRunning = false;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
};

#endif // WIDGET_H
