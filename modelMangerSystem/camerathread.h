#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QImage>
#include "drivers/camera.h"
#include <QMutex>
#include <QElapsedTimer>
#include <QSize>

class CameraThread : public QThread
{
    Q_OBJECT

public:
    explicit CameraThread(QObject *parent = nullptr);
    ~CameraThread();

    void SetCamera(Camera* camera);
    void SetDisplayLabel(QLabel* label);
    void SetDisplayHwnd(void* hwnd) { m_displayHwnd = hwnd; }
    void StopThread();
    void ClearDisplay();
    bool SaveLastFrameToFile(const QString& filePath, int type);
    bool RotateImageToUI(int rotate);
    QImage GetCurrentImage() const { return m_currentImage; }
    void SetTargetSize(const QSize& size);

signals:
    void ImageCaptured(const QImage& image);

protected:
    void run() override;

private:
    Camera* m_camera;
    QLabel* m_displayLabel;
    void* m_displayHwnd = nullptr;
    bool m_isRunning;
    unsigned char* m_convertBuffer;   
    int m_convertBufferSize;
    QPixmap m_buffers[2];           
    int m_currentBuffer;            
    QImage m_currentImage;  
    unsigned char* m_lastFrameBuf = nullptr;
    unsigned int m_lastFrameLen = 0;
    MV_FRAME_OUT_INFO_EX m_lastFrameInfo;
    QMutex m_frameMutex;
    QElapsedTimer m_lastEmitTime; 
    QSize m_targetSize;
};

#endif // CAMERATHREAD_H 
