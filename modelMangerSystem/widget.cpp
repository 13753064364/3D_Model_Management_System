#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QDebug>
#include <QSerialPortInfo>
#include <QFileDialog>
#include <QDateTime>
#include <QGuiApplication>
#include <QScreen>
#include <QDir>
#include <QTimer>
#include <QFileInfo>
#include <algorithm>
#include <QThread>
#include <QApplication>
#include <QCloseEvent>
#include "drivers/ledcontroller.h"
//#include <opencv2/opencv.hpp>
#include <QImage>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , m_ledController(nullptr)
{
    ui->setupUi(this);
    InitUI();
    m_ledController = new LedController(serilaPort, this);
    for (int i = 0; i < 3; ++i) {
        m_cameras[i].InitSDK();
        m_cameraThreads[i] = nullptr;
    }
    m_displayLabels[0] = ui->camera_l;
    m_displayLabels[1] = ui->camera_m;
    m_displayLabels[2] = ui->camera_r;

    for (int i = 0; i < 3; ++i) {
        m_displayLabels[i]->installEventFilter(this);
    }
    
    m_multiModeTimer = new QTimer(this);
    connect(m_multiModeTimer, &QTimer::timeout, this, &Widget::onMultiModeTimerTimeout);
    qDebug() << "多模式拍摄定时器已初始化";
    qDebug() << "定时器对象地址:" << m_multiModeTimer;
    qDebug() << "定时器是否活跃:" << m_multiModeTimer->isActive();
    qDebug() << "定时器间隔:" << m_multiModeTimer->interval() << "毫秒";
}

Widget::~Widget()
{
    if (m_ledController && serilaPort) {
        EnsureSerialPortOpen();
        m_ledController->SetWhite1(0);
        QThread::msleep(20);
        m_ledController->SetWhite2(0);
        QThread::msleep(20);
        m_ledController->SetWhite3(0);
        QThread::msleep(20);
        m_ledController->SetUV1(0);
        QThread::msleep(20);
        m_ledController->SetUV2(0);
        QThread::msleep(20);
        m_ledController->SetBacklight1(0);
        QThread::msleep(20);
        m_ledController->SetBacklight2(0);
        QThread::msleep(20);
        qDebug() << "软件关闭时已完全关闭所有灯光";
        delete m_ledController;
    }
    for (int i = 0; i < 3; ++i) {
        if (m_cameraThreads[i]) {
            m_cameraThreads[i]->StopThread();
            delete m_cameraThreads[i];
        }
    }
    if (m_multiModeTimer) {
        m_multiModeTimer->stop();
    }
    
    delete ui;
}

void Widget::InitUI()
{
//    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
//    this->resize(screenGeometry.size());
//    this->showMaximized();
//    this->resize(1014, 860);
//    this->setStyleSheet("QMainWindow {background-image: url(:/images/1.png);}");

    QStringList serialNamePotr;
    serilaPort = new QSerialPort(this);
    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    if (availablePorts.isEmpty()) {
        qDebug() << "No serial ports available";
    }
    foreach (const QSerialPortInfo &info, availablePorts)
    {
        serialNamePotr<<info.portName();
    }
    ui->SerialCD->addItems(serialNamePotr);//显示自动搜索到的串口号
    ui->Baudrate->setCurrentText("115200");
    ui->DateBit->setCurrentText("8");
    ui->StopBit->setCurrentText("1");
    ui->CheckBit->setCurrentText("none");

//    connect(ui->Open, &QPushButton::clicked, this, &Widget::on_Open_clicked);
//    connect(ui->Close, &QPushButton::clicked, this, &Widget::on_Close_clicked);
//    connect(ui->Send, &QPushButton::clicked, this, &Widget::on_Send_clicked);
    connect(ui->GetButton, &QPushButton::clicked, this, &Widget::on_GetButton_clicked);
    connect(ui->SetButton, &QPushButton::clicked, this, &Widget::on_SetButton_clicked);
    connect(ui->AutoBalanceWhiteButton, &QPushButton::clicked, this, &Widget::on_AutoBalanceWhiteButton_clicked);
//    connect(ui->NaturalLightButton, &QPushButton::clicked, this, &Widget::on_NaturalLightButton_clicked); 
    connect(ui->UVLightButton, &QPushButton::clicked, this, &Widget::on_UVLightButton_clicked);    
    connect(ui->White1Button, &QPushButton::clicked, this, &Widget::on_White1Button_clicked);
    connect(ui->White2Button, &QPushButton::clicked, this, &Widget::on_White2Button_clicked);
    connect(ui->White3Button, &QPushButton::clicked, this, &Widget::on_White3Button_clicked);
    connect(ui->UV1Button, &QPushButton::clicked, this, &Widget::on_UV1Button_clicked);
    connect(ui->UV2Button, &QPushButton::clicked, this, &Widget::on_UV2Button_clicked);
    connect(ui->Backlight1Button, &QPushButton::clicked, this, &Widget::on_Backlight1Button_clicked);
    connect(ui->Backlight2Button, &QPushButton::clicked, this, &Widget::on_Backlight2Button_clicked);
    connect(ui->horizontalSlider, &QSlider::valueChanged, this, &Widget::on_horizontalSlider_valueChanged);
    connect(ui->AllOffButton, &QPushButton::clicked, this, &Widget::on_AllOffButton_clicked);
    connect(ui->FindCamerasButton, &QPushButton::clicked, this, &Widget::on_FindCameras_clicked);
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Widget::on_comboBox_currentIndexChanged);
    connect(ui->IncreaseBrightnessButton, &QPushButton::clicked, this, &Widget::on_IncreaseBrightness_clicked);
    connect(ui->DecreaseBrightnessButton, &QPushButton::clicked, this, &Widget::on_DecreaseBrightness_clicked);
//    connect(ui->MultiModeButton, &QPushButton::clicked, this, &Widget::on_MultiModeButton_clicked);
    qDebug() << "MultiModeButton信号已连接（自动连接）";

    if (serilaPort) {
        connect(serilaPort, &QSerialPort::readyRead, this, &Widget::on_SerialPortReadyRead);
    }
    if (ui->BrightnessValueLabel) {
        ui->BrightnessValueLabel->setText("20"); 
    }
    if (ui->horizontalSlider) {
        ui->horizontalSlider->setRange(0, 150); 
        ui->horizontalSlider->setValue(30); 
    }
    connect(ui->checkBox_20, &QCheckBox::clicked, this, &Widget::onCheckBox20Clicked);
    connect(ui->checkBox_25, &QCheckBox::clicked, this, &Widget::onCheckBox25Clicked);
    connect(ui->checkBox_35, &QCheckBox::clicked, this, &Widget::onCheckBox35Clicked);
//    connect(ui->AutoMultiModeButton, &QPushButton::clicked, this, &Widget::on_AutoMultiModeButton_clicked);
    m_defaultWhiteBrightness = 30; 
    autoOpenSerialPort();
    hideLedTab();
}

void Widget::on_FindCameras_clicked()
{
    ui->comboBox->clear();
    memset(&m_deviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    if (m_cameras[0].EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_deviceList) != 0) {
        QMessageBox::warning(this, "警告", "枚举相机失败");
        return;
    }
    if (m_deviceList.nDeviceNum == 0) {
        QMessageBox::information(this, "提示", "未找到任何相机");
        return;
    }
    for (unsigned int i = 0; i < m_deviceList.nDeviceNum; ++i) {
        MV_CC_DEVICE_INFO* pDeviceInfo = m_deviceList.pDeviceInfo[i];
        QString cameraInfo;
        
        if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE) {
            MV_GIGE_DEVICE_INFO& gigeInfo = pDeviceInfo->SpecialInfo.stGigEInfo;
            cameraInfo = QString("千兆网相机 %1 - %2.%3.%4.%5")
                .arg(i + 1)
                .arg(gigeInfo.nCurrentIp >> 24 & 0xFF)
                .arg(gigeInfo.nCurrentIp >> 16 & 0xFF)
                .arg(gigeInfo.nCurrentIp >> 8 & 0xFF)
                .arg(gigeInfo.nCurrentIp & 0xFF);
        } else if (pDeviceInfo->nTLayerType == MV_USB_DEVICE) {
            MV_USB3_DEVICE_INFO& usbInfo = pDeviceInfo->SpecialInfo.stUsb3VInfo;
            cameraInfo = QString("USB相机 %1 - %2")
                .arg(i + 1)
                .arg(QString::fromLatin1((const char*)usbInfo.chSerialNumber));
        } else {
            cameraInfo = QString("相机 %1 - 未知类型").arg(i + 1);
        }
        ui->comboBox->addItem(cameraInfo, i); // 将索引作为用户数据存储
    }
    QMessageBox::information(this, "查找完成", 
        QString("找到 %1 台相机，已添加到下拉列表中").arg(m_deviceList.nDeviceNum));
}

void Widget::on_comboBox_currentIndexChanged(int index)
{
    if (index < 0 || index >= (int)m_deviceList.nDeviceNum) {
        m_currentCameraIndex = -1;
        return;
    }
    m_currentCameraIndex = ui->comboBox->itemData(index).toInt();
    qDebug() << "切换到相机索引:" << m_currentCameraIndex;
}

void Widget::on_OpenDevice_clicked()
{
    for (int i = 0; i < 3; ++i) {
        if (m_cameraThreads[i]) {
            m_cameraThreads[i]->StopThread();
            m_cameraThreads[i]->wait();
            delete m_cameraThreads[i];
            m_cameraThreads[i] = nullptr;
        }
        m_cameras[i].CloseDevice();
    }
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    if (m_cameras[0].EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList) != 0 || stDeviceList.nDeviceNum == 0) {
        QMessageBox::warning(this, "Warning", "No cameras found");
        return;
    }
    int cameraCount = std::min(stDeviceList.nDeviceNum, (unsigned int)3); 
    qDebug() << "Found" << cameraCount << "cameras";
    for (int i = 0; i < cameraCount; ++i) {
        m_cameras[i].CreateHandle(stDeviceList.pDeviceInfo[i]);
        if (m_cameras[i].OpenDevice() != 0) {
            QMessageBox::warning(this, "Warning", QString("Open camera %1 failed").arg(i + 1));
            continue;
        }
        m_displayLabels[i]->setAttribute(Qt::WA_NativeWindow);
        m_displayLabels[i]->show();
        m_cameraThreads[i] = new CameraThread(this);
        m_cameraThreads[i]->SetCamera(&m_cameras[i]);
        m_cameraThreads[i]->SetDisplayHwnd((void*)m_displayLabels[i]->winId());
        m_cameraThreads[i]->SetTargetSize(m_displayLabels[i]->size());
        connect(m_cameraThreads[i], &CameraThread::ImageCaptured, this, [this, i](const QImage& image) {
            if (m_imageBusy[i]) return; 
            m_imageBusy[i] = true;
            m_latestImages[i] = image;
            if (m_displayLabels[i]) {
                m_displayLabels[i]->setPixmap(QPixmap::fromImage(image));
            }
            m_imageBusy[i] = false;
        }); 
        if (m_cameras[i].StartGrabbing() != 0) {
            QMessageBox::critical(this, "Error", QString("Failed to start grabbing for camera %1").arg(i + 1));
            continue;
        }
        m_cameraThreads[i]->start();
        qDebug() << "Camera" << i + 1 << "started successfully";
    }

    if (cameraCount > 0) {
//        QMessageBox::information(this, "Success", QString("Successfully opened %1 camera(s)").arg(cameraCount));
        qDebug()<<"Successfully opened camera";
    }
}

void Widget::on_CloseDevice_clicked()
{
    for (int i = 0; i < 3; ++i) {
        if (m_cameraThreads[i]) {
            m_cameraThreads[i]->StopThread();
            m_cameraThreads[i]->wait();
            delete m_cameraThreads[i];
            m_cameraThreads[i] = nullptr;
        }
        if (m_cameras[i].IsDeviceConnected()) {
            m_cameras[i].StopGrabbing();
            m_cameras[i].CloseDevice();
        }
        if (m_displayLabels[i]) {
            m_displayLabels[i]->clear();
            m_displayLabels[i]->setText(QString("Camera %1\nClosed").arg(i + 1));
        }
    }
    QMessageBox::information(this, "Success", "All cameras closed successfully");
}

void Widget::on_Open_clicked()
{
    QSerialPort::BaudRate baudrates = QSerialPort::Baud115200; 
    QSerialPort::DataBits databits = QSerialPort::Data8; 
    QSerialPort::StopBits stopbits = QSerialPort::OneStop; 
    QSerialPort::Parity checkbits = QSerialPort::NoParity; 

    if(ui->Baudrate->currentText() == "4800")
    {
        baudrates = QSerialPort::Baud4800;
    }else if(ui->Baudrate->currentText() == "9600")
    {
        baudrates = QSerialPort::Baud9600;
    }else if(ui->Baudrate->currentText() == "115200")
    {
        baudrates = QSerialPort::Baud115200;
    }
    if(ui->DateBit->currentText() == "5")
    {
        databits = QSerialPort::Data5;
    }else if(ui->DateBit->currentText() == "6")
    {
        databits = QSerialPort::Data6;
    }else if(ui->DateBit->currentText() == "7")
    {
        databits = QSerialPort::Data7;
    }else if(ui->DateBit->currentText() == "8")
    {
        databits = QSerialPort::Data8;
    }
    
    if(ui->StopBit->currentText() == "1")
    {
        stopbits = QSerialPort::OneStop;
    }else if(ui->StopBit->currentText() == "1.5")
    {
        stopbits = QSerialPort::OneAndHalfStop;
    }else if(ui->StopBit->currentText() == "2")
    {
        stopbits = QSerialPort::TwoStop;
    }
    
    if(ui->CheckBit->currentText() == "none")
    {
        checkbits = QSerialPort::NoParity;
    }

    serilaPort->setPortName(ui->SerialCD->currentText());
    serilaPort->setBaudRate(baudrates);
    serilaPort->setDataBits(databits);
    serilaPort->setStopBits(stopbits);
    serilaPort->setParity(checkbits);

    if(serilaPort->open(QIODevice::ReadWrite)== true)
    {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("串口打开成功"));
        ui->Open->setDisabled(true);
    }else
    {
        QMessageBox::critical(this, QString::fromUtf8("提示"), QString::fromUtf8("打开失败"));
    }
}

void Widget::on_Close_clicked()
{
    ui->Open->setEnabled(true);
    if (serilaPort && m_ledController) {
        EnsureSerialPortOpen();
        m_ledController->TurnOffAllLights();
        serilaPort->close();
    }
}

void Widget::on_Send_clicked()
{
    if (!serilaPort || !serilaPort->isOpen()) {
        QMessageBox::warning(this, "串口错误", "串口未打开，请先打开串口");
        return;
    }
    QString hexStr = ui->SendEdit->text().remove(' ');
    QByteArray sendData;
    if(hexStr.isEmpty())
        return;
    if(hexStr.length()%2 !=0)
    {
        qDebug()<<"数据输入格式错误";
        return;
    }
    for (int i = 0; i < hexStr.length(); i += 2) {
        bool ok;
        QString byteStr = hexStr.mid(i, 2);
        char byte = byteStr.toUShort(&ok, 16);
        if (ok) {
            sendData.append(byte);
        } else {
            qDebug()<<"数据输入格式错误 ";
            return;
        }
    }
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString hexDisplayStr = QString::fromLatin1(sendData.toHex(' ').toUpper());
    QString sendInfo = QString("[%1] 发送: %2\n").arg(timestamp).arg(hexDisplayStr);
    ui->RecvEdit->appendPlainText(sendInfo);
    
    serilaPort->write(sendData);
}

//void Widget::on_Clean_clicked()
//{
//    ui->RecvEdit->clear();
//}

void Widget::on_SaveButton_clicked()
{
    QDir currentDir = QDir::current();
    QString baseSaveDir = currentDir.filePath("images");
    if (!currentDir.exists("images")) {
        currentDir.mkdir("images");
    }
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString saveDir = QDir(baseSaveDir).filePath(timestamp);
    QDir().mkpath(saveDir);
    int savedCount = 0;
    QStringList savedFiles;
    for (int i = 0; i < 3; ++i) {
        if (m_cameraThreads[i] && m_cameras[i].IsDeviceConnected()) {
            QString cameraName;
            switch(i) {
                case 0: cameraName = "left"; break;
                case 1: cameraName = "center"; break;
                case 2: cameraName = "right"; break;
                default: cameraName = QString("camera%1").arg(i + 1); break;
            }
            QString fileName = QDir(saveDir).filePath(QString("%1.bmp").arg(cameraName));
            bool ok = m_cameraThreads[i]->SaveLastFrameToFile(fileName, 1); // 1=bmp 2=jpg
            if (ok) {
                ++savedCount;
                savedFiles << fileName;
            }
        }
    }
    if (savedCount > 0) {
        QString statusText = QString("✓ 已保存%1张图片到文件夹: %2").arg(savedCount).arg(saveDir);
        ui->StatusLabel->setText(statusText);
        ui->StatusLabel->setStyleSheet("color: green; font-weight: bold;");
        // 5秒后清除状态信息
//        QTimer::singleShot(5000, this, [this]() {
//            ui->StatusLabel->clear();
//        });
        QTimer::singleShot(100, this, [this, savedCount, saveDir, savedFiles]() {
            QMessageBox* msgBox = new QMessageBox(this);
            msgBox->setWindowTitle("保存成功");
            msgBox->setText(QString("已保存%1张图片到文件夹:\n%2\n\n文件列表:\n%3")
                .arg(savedCount)
                .arg(saveDir)
                .arg(savedFiles.join("\n")));
            QPushButton* openFolderBtn = msgBox->addButton("查看文件夹", QMessageBox::ActionRole);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->setModal(false);
            connect(openFolderBtn, &QPushButton::clicked, this, [this, saveDir, msgBox]() {
                openSavedFolder(saveDir);
                msgBox->close();
            });
            msgBox->show();
            // 5秒后自动关闭消息框
//            QTimer::singleShot(5000, msgBox, &QMessageBox::close);
        });
    } else {
        ui->StatusLabel->setText("✗ 保存失败：未检测到可用的相机");
        ui->StatusLabel->setStyleSheet("color: red; font-weight: bold;");
        // 5秒后清除状态信息
//        QTimer::singleShot(5000, this, [this]() {
//            ui->StatusLabel->clear();
//        });
    }
}

void Widget::on_GetButton_clicked()
{
    if (m_currentCameraIndex < 0 || m_currentCameraIndex >= 3) {
        QMessageBox::warning(this, "获取失败", "请先在下拉列表中选择一个相机");
        return;
    }
    if (!m_cameras[m_currentCameraIndex].IsDeviceConnected()) {
        QMessageBox::warning(this, "获取失败", "选中的相机未连接，请先打开相机");
        return;
    }
    MVCC_FLOATVALUE expValue = {0};
    if (m_cameras[m_currentCameraIndex].GetFloatValue("ExposureTime", &expValue) == 0)
        ui->ExposureLineEdit->setText(QString::number(expValue.fCurValue, 'f', 2));
    MVCC_FLOATVALUE gainValue = {0};
    if (m_cameras[m_currentCameraIndex].GetFloatValue("Gain", &gainValue) == 0)
        ui->GainLineEdit->setText(QString::number(gainValue.fCurValue, 'f', 2));
    m_cameras[m_currentCameraIndex].SetEnumValueByString("BalanceRatioSelector", "Red");
    MVCC_INTVALUE wbRed = {0};
    if (m_cameras[m_currentCameraIndex].GetIntValue("BalanceRatio", &wbRed) == 0)
        ui->BalanceWhiteLineEdit_3->setText(QString::number(wbRed.nCurValue));
    else
        ui->BalanceWhiteLineEdit_3->setText("N/A");
}

void Widget::on_SetButton_clicked()
{
    if (m_currentCameraIndex < 0 || m_currentCameraIndex >= 3) {
        QMessageBox::warning(this, "设置失败", "请先在下拉列表中选择一个相机");
        return;
    }
    if (!m_cameras[m_currentCameraIndex].IsDeviceConnected()) {
        QMessageBox::warning(this, "设置失败", "选中的相机未连接，请先打开相机");
        return;
    }
    bool ok1 = false, ok2 = false, ok3 = false;
    double exposure = ui->ExposureLineEdit->text().toDouble(&ok1);
    double gain = ui->GainLineEdit->text().toDouble(&ok2);
    int wb = ui->BalanceWhiteLineEdit_3->text().toInt(&ok3);
    if (ok1) {
        m_cameras[m_currentCameraIndex].SetEnumValue("ExposureAuto", 0); 
        m_cameras[m_currentCameraIndex].SetFloatValue("ExposureTime", (float)exposure);
    }
    if (ok2) {
        m_cameras[m_currentCameraIndex].SetEnumValue("GainAuto", 0); 
        m_cameras[m_currentCameraIndex].SetFloatValue("Gain", (float)gain);
    }
    if (ok3) {
        m_cameras[m_currentCameraIndex].SetEnumValue("BalanceWhiteAuto", 0); 
        m_cameras[m_currentCameraIndex].SetEnumValueByString("BalanceRatioSelector", "Red");
        m_cameras[m_currentCameraIndex].SetIntValue("BalanceRatio", wb);
    }
}

void Widget::on_AutoBalanceWhiteButton_clicked()
{
    if (m_currentCameraIndex < 0 || m_currentCameraIndex >= 3) {
        QMessageBox::warning(this, "自动白平衡失败", "请先在下拉列表中选择一个相机");
        return;
    }
    if (!m_cameras[m_currentCameraIndex].IsDeviceConnected()) {
        QMessageBox::warning(this, "自动白平衡失败", "选中的相机未连接，请先打开相机");
        return;
    }
    int result = m_cameras[m_currentCameraIndex].SetEnumValue("BalanceWhiteAuto", 1); 
    if (result == 0) {
        QMessageBox::information(this, "成功", QString("已为相机 %1 执行自动白平衡").arg(m_currentCameraIndex + 1));
    } else {
        QMessageBox::warning(this, "失败", QString("为相机 %1 执行自动白平衡失败").arg(m_currentCameraIndex + 1));
    }
}

void Widget::EnsureSerialPortOpen()
{
    if (!serilaPort) {
        qDebug() << "Serial port object is null";
        return;
    }
    if (serilaPort->isOpen()) {
        return;
    }
    autoOpenSerialPort();
}

//void Widget::on_NaturalLightButton_clicked()
//{
//    m_selectedLightGroup = LedController::LightGroup::White1;
//    if (m_ledController) {
//        int brightness = 20; 
//        m_ledController->SetWhite1(brightness);
//        m_ledController->SetWhite2(brightness);
//        m_ledController->SetWhite3(brightness);
//        m_ledController->SetUV1(0);
//        m_ledController->SetUV2(0);
//        ui->horizontalSlider->setValue(brightness);
//        if (ui->BrightnessValueLabel) {
//            ui->BrightnessValueLabel->setText(QString::number(brightness));
//        }
//        m_ledController->SetCurrentBrightness(brightness);
//    }
//}

void Widget::on_UVLightButton_clicked()
{
    m_selectedLightGroup = LedController::LightGroup::UV1;
    if (m_ledController) {
        EnsureSerialPortOpen();
        int brightness = 150; 
        m_ledController->SetWhite1(0);
        m_ledController->SetWhite2(0);
        m_ledController->SetWhite3(0);
        m_ledController->SetUV1(brightness);
        m_ledController->SetUV2(brightness);
        ui->horizontalSlider->setValue(brightness);
        if (ui->BrightnessValueLabel) {
            ui->BrightnessValueLabel->setText(QString::number(brightness));
        }
        m_ledController->SetCurrentBrightness(brightness);
    }
}

void Widget::on_White1Button_clicked()
{
    m_selectedLightGroup = LedController::LightGroup::White1;
    if (m_ledController) {
        EnsureSerialPortOpen();
        int brightness = m_defaultWhiteBrightness;
        m_ledController->SetWhite1(brightness);
        m_ledController->SetWhite2(0);
        m_ledController->SetWhite3(0);
        m_ledController->SetUV1(0);
        m_ledController->SetUV2(0);
        ui->horizontalSlider->setValue(brightness);
        if (ui->BrightnessValueLabel) {
            ui->BrightnessValueLabel->setText(QString::number(brightness));
        }
        m_ledController->SetCurrentBrightness(brightness);
    }
}

void Widget::on_White2Button_clicked()
{
    m_selectedLightGroup = LedController::LightGroup::White2;
    if (m_ledController) {
        EnsureSerialPortOpen();
        int brightness = m_defaultWhiteBrightness; 
        m_ledController->SetWhite1(0);
        m_ledController->SetWhite2(brightness);
        m_ledController->SetWhite3(0);
        m_ledController->SetUV1(0);
        m_ledController->SetUV2(0);
        ui->horizontalSlider->setValue(brightness);
        if (ui->BrightnessValueLabel) {
            ui->BrightnessValueLabel->setText(QString::number(brightness));
        }
        m_ledController->SetCurrentBrightness(brightness);
    }
}

void Widget::on_White3Button_clicked()
{
    m_selectedLightGroup = LedController::LightGroup::White3;
    if (m_ledController) {
        EnsureSerialPortOpen();
        int brightness = m_defaultWhiteBrightness;
        m_ledController->SetWhite1(0);
        m_ledController->SetWhite2(0);
        m_ledController->SetWhite3(brightness);
        m_ledController->SetUV1(0);
        m_ledController->SetUV2(0);
        ui->horizontalSlider->setValue(brightness);
        if (ui->BrightnessValueLabel) {
            ui->BrightnessValueLabel->setText(QString::number(brightness));
        }
        m_ledController->SetCurrentBrightness(brightness);
    }
}

void Widget::on_UV1Button_clicked()
{
    m_selectedLightGroup = LedController::LightGroup::UV1;
    if (m_ledController) {
        EnsureSerialPortOpen();
        int brightness = 150; 
        m_ledController->SetWhite1(0);
        m_ledController->SetWhite2(0);
        m_ledController->SetWhite3(0);
        m_ledController->SetUV1(brightness);
        m_ledController->SetUV2(0);
        ui->horizontalSlider->setValue(brightness);
        if (ui->BrightnessValueLabel) {
            ui->BrightnessValueLabel->setText(QString::number(brightness));
        }
        m_ledController->SetCurrentBrightness(brightness);
    }
}

void Widget::on_UV2Button_clicked()
{
    m_selectedLightGroup = LedController::LightGroup::UV2;
    if (m_ledController) {
        EnsureSerialPortOpen();
        int brightness = 150; 
        m_ledController->SetWhite1(0);
        m_ledController->SetWhite2(0);
        m_ledController->SetWhite3(0);
        m_ledController->SetUV1(0);
        m_ledController->SetUV2(brightness);
        ui->horizontalSlider->setValue(brightness);
        if (ui->BrightnessValueLabel) {
            ui->BrightnessValueLabel->setText(QString::number(brightness));
        }
        m_ledController->SetCurrentBrightness(brightness);
    }
}

void Widget::on_Backlight1Button_clicked()
{
    m_selectedLightGroup = LedController::LightGroup::Backlight1;
    if (m_ledController) {
        EnsureSerialPortOpen();
        int brightness = m_defaultWhiteBrightness;
        m_ledController->SetBacklight1(brightness);
        ui->horizontalSlider->setValue(brightness);
        if (ui->BrightnessValueLabel) {
            ui->BrightnessValueLabel->setText(QString::number(brightness));
        }
        m_ledController->SetCurrentBrightness(brightness);
    }
}
void Widget::on_Backlight2Button_clicked()
{
    m_selectedLightGroup = LedController::LightGroup::Backlight2;
    if (m_ledController) {
        EnsureSerialPortOpen();
        int brightness = m_defaultWhiteBrightness;
        m_ledController->SetBacklight2(brightness);
        ui->horizontalSlider->setValue(brightness);
        if (ui->BrightnessValueLabel) {
            ui->BrightnessValueLabel->setText(QString::number(brightness));
        }
        m_ledController->SetCurrentBrightness(brightness);
    }
}

void Widget::on_horizontalSlider_valueChanged(int value)
{
    if (ui->BrightnessValueLabel) {
        ui->BrightnessValueLabel->setText(QString::number(value));
    }
    if (m_ledController) {
        EnsureSerialPortOpen();
        m_ledController->SetCurrentBrightness(value);
        m_ledController->SetLightLevel(m_selectedLightGroup, value);
    }
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    for (int i = 0; i < 3; ++i) {
        if (watched == m_displayLabels[i] && event->type() == QEvent::Resize) {
            if (m_cameraThreads[i]) {
//                m_cameraThreads[i]->setTargetSize(m_displayLabels[i]->size());
                m_cameraThreads[i]->SetTargetSize(m_displayLabels[i]->contentsRect().size());
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void Widget::on_AllOffButton_clicked()
{
    if (m_ledController) {
        EnsureSerialPortOpen();
        m_ledController->TurnOffAllLights();
        ui->horizontalSlider->setValue(0);
        if (ui->BrightnessValueLabel) {
            ui->BrightnessValueLabel->setText("0");
        }
        m_ledController->SetCurrentBrightness(0);
    }
}

void Widget::on_IncreaseBrightness_clicked()
{
    if (m_ledController) {
        EnsureSerialPortOpen();
        m_ledController->IncreaseBrightness();
        int newValue = m_ledController->GetCurrentBrightness();
        ui->horizontalSlider->setValue(newValue);
        if (ui->BrightnessValueLabel) {
            ui->BrightnessValueLabel->setText(QString::number(newValue));
        }
        if (m_selectedLightGroup == LedController::LightGroup::UV1 ||
            m_selectedLightGroup == LedController::LightGroup::UV2) {
            m_ledController->SetLightLevel(LedController::LightGroup::UV1, newValue);
            m_ledController->SetLightLevel(LedController::LightGroup::UV2, newValue);
        } else {
            m_ledController->SetLightLevel(m_selectedLightGroup, newValue);
        }
    }
}

void Widget::on_DecreaseBrightness_clicked()
{
    if (m_ledController) {
        EnsureSerialPortOpen();
        m_ledController->DecreaseBrightness();
        int newValue = m_ledController->GetCurrentBrightness();
        ui->horizontalSlider->setValue(newValue);
        if (ui->BrightnessValueLabel) {
            ui->BrightnessValueLabel->setText(QString::number(newValue));
        }
        if (m_selectedLightGroup == LedController::LightGroup::UV1 ||
            m_selectedLightGroup == LedController::LightGroup::UV2) {
            m_ledController->SetLightLevel(LedController::LightGroup::UV1, newValue);
            m_ledController->SetLightLevel(LedController::LightGroup::UV2, newValue);
        } else {
            m_ledController->SetLightLevel(m_selectedLightGroup, newValue);
        }
    }
}

void Widget::on_SerialPortReadyRead()
{
    if (!serilaPort || !serilaPort->isOpen()) {
        return;
    }
    QByteArray data = serilaPort->readAll();
    if (!data.isEmpty()) {
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        QByteArray hexArray = data.toHex().toUpper();
        QStringList groups;
        for (int i = 0; i < hexArray.size(); i += 16) { 
            groups << QString::fromLatin1(hexArray.mid(i, 16));
        }
        QString hexStr = groups.join(" ");
        QString receivedData = QString("[%1] 接收: %2").arg(timestamp).arg(hexStr);
        ui->RecvEdit->appendPlainText(receivedData);
    }
}

void Widget::on_OneClickFillLightButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择图片文件夹", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        return;
    }
    QDir imageDir(dir);
    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp";
    QFileInfoList fileList = imageDir.entryInfoList(nameFilters, QDir::Files | QDir::NoSymLinks);
    if (fileList.isEmpty()) {
        QMessageBox::warning(this, "提示", "该文件夹下没有图片文件！");
        return;
    }
    QString saveDir = QFileDialog::getExistingDirectory(this, "选择补光图片保存文件夹", dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (saveDir.isEmpty()) {
        return;
    }
    int successCount = 0;
    for (const QFileInfo &fileInfo : fileList) {
        QString imagePath = fileInfo.absoluteFilePath();
        QImage image(imagePath);
        if (image.isNull()) continue;
//        QImage filledImage = fillLight(image); 
//        QString savePath = QDir(saveDir).filePath(fileInfo.fileName());
//        if (filledImage.save(savePath)) {
//            ++successCount;
//        }
    }
    QMessageBox::information(this, "一键补光完成", QString("共处理%1张图片，已保存补光结果到所选文件夹\n").arg(successCount));
}

void Widget::openSavedFolder(const QString& path)
{
#ifdef Q_OS_WIN
    QProcess::startDetached("explorer", QStringList() << QDir::toNativeSeparators(path));
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
#endif
}

void Widget::onCheckBox20Clicked()
{
    if (ui->checkBox_20->isChecked()) {
        ui->checkBox_25->setChecked(false);
        ui->checkBox_35->setChecked(false);
        m_defaultWhiteBrightness = 20;
    } else if (!ui->checkBox_25->isChecked() && !ui->checkBox_35->isChecked()) {
        m_defaultWhiteBrightness = 30;
    }
}
void Widget::onCheckBox25Clicked()
{
    if (ui->checkBox_25->isChecked()) {
        ui->checkBox_20->setChecked(false);
        ui->checkBox_35->setChecked(false);
        m_defaultWhiteBrightness = 25;
    } else if (!ui->checkBox_20->isChecked() && !ui->checkBox_35->isChecked()) {
        m_defaultWhiteBrightness = 30;
    }
}
void Widget::onCheckBox35Clicked()
{
    if (ui->checkBox_35->isChecked()) {
        ui->checkBox_20->setChecked(false);
        ui->checkBox_25->setChecked(false);
        m_defaultWhiteBrightness = 35;
    } else if (!ui->checkBox_20->isChecked() && !ui->checkBox_25->isChecked()) {
        m_defaultWhiteBrightness = 30;
    }
}

void Widget::on_MultiModeButton_clicked()
{
//    qDebug() << "=== 多模式按钮被点击 ===";
    qDebug() << "当前多模式运行状态:" << m_isMultiModeRunning;
//    qDebug() << "定时器是否活跃:" << (m_multiModeTimer ? m_multiModeTimer->isActive() : false);
    
    if (m_isMultiModeRunning) {
        qDebug() << "停止多模式拍摄";
        m_multiModeTimer->stop();
        m_isMultiModeRunning = false;
//        ui->AutoMultiModeButton->setText("一键多模式拍摄");
        ui->StatusLabel->setText("多模式拍摄已停止");
        qDebug()<<"多模式拍摄已停止";
        ui->StatusLabel->setStyleSheet("color: orange; font-weight: bold;");
        return;
    }
    bool hasConnectedCamera = false;
    for (int i = 0; i < 3; ++i) {
        if (m_cameras[i].IsDeviceConnected()) {
            hasConnectedCamera = true;
            break;
        }
    }
    if (!hasConnectedCamera) {
        QMessageBox::warning(this, "警告", "请先打开相机设备");
        return;
    }
    qDebug() << "开始多模式拍摄流程";
    QDir currentDir = QDir::current();
    QString baseSaveDir = currentDir.filePath("images");
    if (!currentDir.exists("images")) {
        currentDir.mkdir("images");
    }
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    m_multiModeSaveDir = QDir(baseSaveDir).filePath(QString("multi_mode_%1").arg(timestamp));
    QDir().mkpath(m_multiModeSaveDir);
    m_currentModeIndex = 0;
    m_isMultiModeRunning = true;
//    ui->AutoMultiModeButton->setText("停止多模式拍摄");
    ui->StatusLabel->setText("开始多模式拍摄...");
    ui->StatusLabel->setStyleSheet("color: green; font-weight: bold;");
    qDebug() << "开始多模式拍摄，保存目录:" << m_multiModeSaveDir;
    qDebug() << "当前模式索引:" << m_currentModeIndex;
    startNextMode();
}

void Widget::onMultiModeTimerTimeout()
{
    qDebug() << "当前时间:" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    qDebug() << "定时器是否活跃:" << m_multiModeTimer->isActive();
    qDebug() << "多模式运行状态:" << m_isMultiModeRunning;
    qDebug() << "定时器超时，当前模式索引:" << m_currentModeIndex;
    
    saveCurrentModeImages();
    m_currentModeIndex++;
    qDebug() << "切换到下一个模式，新索引:" << m_currentModeIndex;
    
    if (m_currentModeIndex >= 4) {
        qDebug() << "所有模式完成，停止定时器";
        m_multiModeTimer->stop();
        m_isMultiModeRunning = false;
//        ui->AutoMultiModeButton->setText("一键多模式拍摄");
        ui->StatusLabel->setText("多模式拍摄完成！");
        ui->StatusLabel->setStyleSheet("color: green; font-weight: bold;");
        if (m_ledController) {
            m_ledController->TurnOffAllLights();
            qDebug() << "关闭所有灯光";
        }
        QMessageBox::information(this, "拍摄完成", 
            QString("多模式拍摄完成！\n已保存12张图片到文件夹:\n%1").arg(m_multiModeSaveDir));
        openSavedFolder(m_multiModeSaveDir);
    } else {
        qDebug() << "继续下一个模式";
        startNextMode();
    }
}

void Widget::startNextMode()
{
    qDebug() << "=== 开始下一个模式 ===";
    qDebug() << "当前模式索引:" << m_currentModeIndex;
    qDebug() << "LED控制器是否存在:" << (m_ledController != nullptr);
    
    if (!m_ledController) return;
    EnsureSerialPortOpen();
    qDebug() << "串口状态检查 - 串口对象:" << (serilaPort != nullptr);
    qDebug() << "串口是否打开:" << (serilaPort ? serilaPort->isOpen() : false);
    if (serilaPort && serilaPort->isOpen()) {
        qDebug() << "串口名称:" << serilaPort->portName();
    }
    m_ledController->SetWhite1(0); QThread::msleep(30);
    m_ledController->SetWhite2(0); QThread::msleep(30);
    m_ledController->SetWhite3(0); QThread::msleep(30);
    m_ledController->SetUV1(0);    QThread::msleep(30);
    m_ledController->SetUV2(0);    QThread::msleep(30);
    m_ledController->SetBacklight1(0); QThread::msleep(30);
    m_ledController->SetBacklight2(0); QThread::msleep(30);
    qDebug() << "已依次关闭所有灯光";
    int brightness = m_defaultWhiteBrightness;
    m_ledController->SetBacklight1(brightness); QThread::msleep(30);
    qDebug()<<"打开背光";
    ui->horizontalSlider->setValue(brightness);
    if (ui->BrightnessValueLabel) {
        ui->BrightnessValueLabel->setText(QString::number(brightness));
    }
    m_ledController->SetCurrentBrightness(brightness);
    QApplication::processEvents();
    QTimer::singleShot(200, this, [this]() {
        switch (m_currentModeIndex) {
            case 0: 
                m_ledController->SetWhite3(m_defaultWhiteBrightness); QThread::msleep(30);
                ui->StatusLabel->setText("模式1/4: 自然光模式");
                qDebug() << "设置自然光模式 - White3:" << m_defaultWhiteBrightness;
                break;
            case 1: 
                m_ledController->SetWhite2(m_defaultWhiteBrightness); QThread::msleep(30);
                ui->StatusLabel->setText("模式2/4: 交叉偏振模式");
                qDebug() << "设置交叉偏振模式 - White2:" << m_defaultWhiteBrightness;
                break;
            case 2: 
                m_ledController->SetWhite1(m_defaultWhiteBrightness); QThread::msleep(30);
                ui->StatusLabel->setText("模式3/4: 平行偏振模式");
                qDebug() << "设置平行偏振模式 - White1:" << m_defaultWhiteBrightness;
                break;
            case 3: 
                m_ledController->SetUV1(150); QThread::msleep(30);
                m_ledController->SetUV2(150); QThread::msleep(30);
                ui->StatusLabel->setText("模式4/4: UV光模式");
                qDebug() << "设置UV光模式 - UV1:150 UV2:150";
                break;
        }
        QApplication::processEvents();
        QTimer::singleShot(300, this, [this]() {
            qDebug() << "启动定时器，2秒后拍摄模式" << m_currentModeIndex + 1;
            qDebug() << "定时器启动前状态:" << m_multiModeTimer->isActive();
            m_multiModeTimer->start(2000);
            qDebug() << "定时器启动后状态:" << m_multiModeTimer->isActive();
            qDebug() << "定时器间隔:" << m_multiModeTimer->interval() << "毫秒";
        });
    });
}

void Widget::saveCurrentModeImages()
{
    QString modePrefix;
    switch (m_currentModeIndex) {
        case 0: modePrefix = "natural"; break;      
        case 1: modePrefix = "cross_polar"; break;  
        case 2: modePrefix = "parallel_polar"; break; 
        case 3: modePrefix = "uv"; break;        
        default: modePrefix = "unknown"; break;
    }
    qDebug() << "开始保存模式" << m_currentModeIndex + 1 << "的图片，前缀:" << modePrefix;
    qDebug() << "保存目录:" << m_multiModeSaveDir;
    int savedCount = 0;
    for (int i = 0; i < 3; ++i) {
        if (m_cameraThreads[i] && m_cameras[i].IsDeviceConnected()) {
            QString cameraName;
            switch(i) {
                case 0: cameraName = "left"; break;
                case 1: cameraName = "center"; break;
                case 2: cameraName = "right"; break;
                default: cameraName = QString("camera%1").arg(i + 1); break;
            }
            QString fileName = QDir(m_multiModeSaveDir).filePath(
                QString("%1_%2_%3.bmp").arg(modePrefix).arg(cameraName).arg(m_currentModeIndex + 1));
            qDebug() << "尝试保存相机" << i << "图片到:" << fileName;
            QFileInfo fileInfo(fileName);
            QDir dir = fileInfo.dir();
            if (!dir.exists()) {
                qDebug() << "目录不存在，创建目录:" << dir.path();
                dir.mkpath(".");
            }
            bool ok = m_cameraThreads[i]->SaveLastFrameToFile(fileName, 1); // 1=bmp
            if (ok) {
                ++savedCount;
                qDebug() << "成功保存相机" << i << "图片";
            } else {
                qDebug() << "失败保存相机" << i << "图片";
            }
        } else {
            qDebug() << "相机" << i << "未连接或线程不存在";
        }
    }
    qDebug() << QString("模式%1: 保存了%2张图片").arg(m_currentModeIndex + 1).arg(savedCount);
}

void Widget::autoOpenSerialPort()
{
    if (!serilaPort) {
        qDebug() << "Serial port object is null";
        return;
    }
    if (serilaPort->isOpen()) {
        qDebug() << "Serial port is already open";
        return;
    }
    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    if (availablePorts.isEmpty()) {
        qDebug() << "No serial ports available for auto-opening";
        return;
    }
    QString selectedPort;
    if (!ui->SerialCD->currentText().isEmpty()) {
        selectedPort = ui->SerialCD->currentText();
    } else if (!availablePorts.isEmpty()) {
        selectedPort = availablePorts.first().portName();
        ui->SerialCD->setCurrentText(selectedPort);
    } else {
        qDebug() << "No serial port selected and no available ports";
        return;
    }
    QSerialPort::BaudRate baudrates = QSerialPort::Baud115200;
    QSerialPort::DataBits databits = QSerialPort::Data8;
    QSerialPort::StopBits stopbits = QSerialPort::OneStop;
    QSerialPort::Parity checkbits = QSerialPort::NoParity;
    
    serilaPort->setPortName(selectedPort);
    serilaPort->setBaudRate(baudrates);
    serilaPort->setDataBits(databits);
    serilaPort->setStopBits(stopbits);
    serilaPort->setParity(checkbits);
    
    if (serilaPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port opened successfully:" << selectedPort;
        if (ui->Open) {
            ui->Open->setEnabled(false); 
        }
        if (ui->Close) {
            ui->Close->setEnabled(true); 
        }
    } else {
        qDebug() << "Failed to open serial port:" << selectedPort << "Error:" << serilaPort->errorString();
        for (const QSerialPortInfo &info : availablePorts) {
            if (info.portName() != selectedPort) {
                serilaPort->setPortName(info.portName());
                if (serilaPort->open(QIODevice::ReadWrite)) {
                    qDebug() << "Serial port opened successfully on alternative port:" << info.portName();
                    ui->SerialCD->setCurrentText(info.portName());
                    if (ui->Open) {
                        ui->Open->setEnabled(false);
                    }
                    if (ui->Close) {
                        ui->Close->setEnabled(true);
                    }
                    break;
                }
            }
        }
    }
}

void Widget::hideLedTab()
{
    QTabWidget* tabWidget = findChild<QTabWidget*>("StartGrabbing");
    if (tabWidget) {
        for (int i = 0; i < tabWidget->count(); ++i) {
            QWidget* tabPage = tabWidget->widget(i);
            if (tabPage && tabPage->objectName() == "led") {
                tabWidget->removeTab(i);
                qDebug() << "LED tab页已隐藏";
                break;
            }
        }
    } else {
        qDebug() << "未找到StartGrabbing tab widget";
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    if (m_ledController && serilaPort) {
        EnsureSerialPortOpen();
        m_ledController->SetWhite1(0);
        QThread::msleep(20);
        m_ledController->SetWhite2(0);
        QThread::msleep(20);
        m_ledController->SetWhite3(0);
        QThread::msleep(20);
        m_ledController->SetUV1(0);
        QThread::msleep(20);
        m_ledController->SetUV2(0);
        QThread::msleep(20);
        m_ledController->SetBacklight1(0);
        QThread::msleep(20);
        m_ledController->SetBacklight2(0);
        QThread::msleep(20);
        qDebug() << "窗口关闭时已完全关闭所有灯光";
    }
    if (m_multiModeTimer && m_multiModeTimer->isActive()) {
        m_multiModeTimer->stop();
        qDebug() << "窗口关闭时停止多模式拍摄定时器";
    }
    event->accept();
}


