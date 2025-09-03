#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <QObject>

class QSerialPort;

//struct LED_Protocol
//{
//    HEAD1,
//    HEAD2,
//    LENGTH,
//    CMD1,
//    CMD2,
//    CTRL,
//    NUMBER,
//    LEVEL1,
//    LEVEL2,
//    LEVEL3
//};

class LedController : public QObject
{
    Q_OBJECT
public:
    enum class LightGroup {
        White1 = 0x31,
        White2 = 0x32,
        White3 = 0x33,
        UV1 = 0x34,
        UV2 = 0x35,
        Backlight1 = 0x36,
        Backlight2 = 0x37
    };
    Q_ENUM(LightGroup)

    explicit LedController(QSerialPort *serialPort, QObject *parent = nullptr);
    ~LedController();

    void SetLightLevel(LightGroup group, int level);
    void TurnOffAllLights();
    void SetWhite1(int level);
    void SetWhite2(int level);
    void SetWhite3(int level);
    void SetUV1(int level);
    void SetUV2(int level);
    void SetBacklight1(int level);
    void SetBacklight2(int level);
    
    void IncreaseBrightness();
    void DecreaseBrightness();
    int GetCurrentBrightness() const { return m_currentBrightness; }
    void SetCurrentBrightness(int brightness) { m_currentBrightness = brightness; }

private:
    QSerialPort *m_serialPort;
    int m_currentBrightness = 20; 
};

#endif // LEDCONTROLLER_H 
