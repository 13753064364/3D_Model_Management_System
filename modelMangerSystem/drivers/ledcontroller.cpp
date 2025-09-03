#include "ledcontroller.h"
#include <QDebug>
#include <QSerialPort>
#include <QThread> 

LedController::LedController(QSerialPort *serialPort, QObject *parent)
    : QObject(parent), m_serialPort(serialPort)
{
    if (!m_serialPort) {
        qDebug() << "Warning: LED controller created with null serial port";
    }
}

LedController::~LedController()
{
}

void LedController::SetLightLevel(LightGroup group, int level)
{
    if (!m_serialPort) {
        qDebug() << "Serial port is null for LED controller.";
        return;
    }
    if (!m_serialPort->isOpen()) {
        qDebug() << "Serial port is not open, attempting to open...";
        if (!m_serialPort->open(QIODevice::ReadWrite)) {
            qDebug() << "Failed to open serial port for LED control:" << m_serialPort->errorString();
            return;
        }
        qDebug() << "Serial port opened successfully for LED control";
    }

    if (level < 0) level = 0;
    if (level > 150) level = 150;

    QByteArray command;
    command.append(static_cast<char>(0x5A));
    command.append(static_cast<char>(0xA5));
    command.append(static_cast<char>(0x07));
    command.append(static_cast<char>(0x4C));
    command.append(static_cast<char>(0x44));
    command.append(static_cast<char>(0x44));
    command.append(static_cast<char>(group));

    QString levelStr = QString("%1").arg(level, 3, 10, QChar('0'));
    command.append(levelStr.toUtf8());

    qDebug() << "准备发送LED命令 - Group:" << static_cast<int>(group) << "Level:" << level;
    qDebug() << "命令长度:" << command.length() << "字节";
    qDebug() << "命令内容:" << command.toHex(' ');
    
    m_serialPort->write(command);
    qDebug() << "Sent LED command:" << command.toHex(' ');
    qDebug() << "LED Group:" << static_cast<int>(group) << "Level:" << level;
    
    if (m_serialPort->waitForBytesWritten(1000)) {
        qDebug() << "LED命令写入成功";
    } else {
        qDebug() << "LED命令写入超时";
    }
    QThread::msleep(20);
}


void LedController::SetWhite1(int level)
{
    SetLightLevel(LightGroup::White1, level);
}
void LedController::SetWhite2(int level)
{
    SetLightLevel(LightGroup::White2, level);
}
void LedController::SetWhite3(int level)
{
    SetLightLevel(LightGroup::White3, level);
}
void LedController::SetUV1(int level)
{
    SetLightLevel(LightGroup::UV1, level);
}
void LedController::SetUV2(int level)
{
    SetLightLevel(LightGroup::UV2, level);
}
void LedController::SetBacklight1(int level)
{
    SetLightLevel(LightGroup::Backlight1,level);
}
void LedController::SetBacklight2(int level)
{
    SetLightLevel(LightGroup::Backlight2,level);
}

void LedController::TurnOffAllLights()
{
    qDebug() << "开始关闭所有灯光";
    SetLightLevel(LightGroup::White1, 0);
    SetLightLevel(LightGroup::White2, 0);
    SetLightLevel(LightGroup::White3, 0);
    SetLightLevel(LightGroup::UV1, 0);
    SetLightLevel(LightGroup::UV2, 0);
    SetLightLevel(LightGroup::Backlight1, 0);
    SetLightLevel(LightGroup::Backlight2, 0);
    qDebug() << "所有灯光关闭完成";
}

void LedController::IncreaseBrightness()
{
    if (m_currentBrightness < 150) {
        m_currentBrightness += 5; // 每次增加5
        if (m_currentBrightness > 150) {
            m_currentBrightness = 150;
        }
        qDebug() << "Increased brightness to:" << m_currentBrightness;
    }
}

void LedController::DecreaseBrightness()
{
    if (m_currentBrightness > 0) {
        m_currentBrightness -= 5; // 每次减少5
        if (m_currentBrightness < 0) {
            m_currentBrightness = 0;
        }
        qDebug() << "Decreased brightness to:" << m_currentBrightness;
    }
} 
