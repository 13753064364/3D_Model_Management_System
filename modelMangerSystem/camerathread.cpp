#include "camerathread.h"
#include <QDebug>
#include <QCoreApplication>
#include <QSize>

CameraThread::CameraThread(QObject *parent)
    : QThread(parent)
    , m_camera(nullptr)
    , m_displayLabel(nullptr)
    , m_isRunning(false)
    , m_convertBuffer(nullptr)
    , m_convertBufferSize(0)
    , m_currentBuffer(0)
    , m_lastFrameBuf(nullptr)
    , m_lastFrameLen(0)
    , m_targetSize()
{
//    setPriority(QThread::HighPriority);
}

CameraThread::~CameraThread()
{
    StopThread();
    if (m_convertBuffer) {
        free(m_convertBuffer);
        m_convertBuffer = nullptr;
    }
    if (m_lastFrameBuf) {
        free(m_lastFrameBuf);
        m_lastFrameBuf = nullptr;
    }
}

void CameraThread::SetCamera(Camera* camera)
{
    m_camera = camera;
}

void CameraThread::SetDisplayLabel(QLabel* label)
{
    m_displayLabel = label;
    m_buffers[0] = QPixmap(label->size());
    m_buffers[1] = QPixmap(label->size());
}

void CameraThread::ClearDisplay()
{
    if (m_displayLabel) {
        m_displayLabel->clear();
    }
}

void CameraThread::StopThread()
{
    m_isRunning = false;
    wait();
    ClearDisplay();
}

void CameraThread::SetTargetSize(const QSize& size)
{
    m_targetSize = size;
}

void CameraThread::run()
{
    if (!m_camera) {
        return;
    }
    m_isRunning = true;
    MV_FRAME_OUT stFrame;
    memset(&stFrame, 0, sizeof(MV_FRAME_OUT));
    m_camera->SetBayerCvtQuality(1);
//    m_lastEmitTime.start(); // 新增：启动限流计时器
    while (m_isRunning) {
        int ret = m_camera->GetImageBuffer(&stFrame, 1000);
        if (ret == 0) {
            if (m_displayHwnd) {
                MV_CC_IMAGE stImageData;
                stImageData.nWidth = stFrame.stFrameInfo.nExtendWidth;
                stImageData.nHeight = stFrame.stFrameInfo.nExtendHeight;
                stImageData.enPixelType = stFrame.stFrameInfo.enPixelType;
                stImageData.pImageBuf = (unsigned char*)stFrame.pBufAddr;
                stImageData.nImageLen = stFrame.stFrameInfo.nFrameLenEx;
                stImageData.nImageBufSize = stFrame.stFrameInfo.nFrameLenEx;
                memset(stImageData.nReserved, 0, sizeof(stImageData.nReserved));
                m_camera->DisplayOneFrame(m_displayHwnd, &stImageData);
            }            
            {
                QMutexLocker locker(&m_frameMutex);
                if (m_lastFrameBuf) {
                    free(m_lastFrameBuf);
                    m_lastFrameBuf = nullptr;
                }
                m_lastFrameLen = stFrame.stFrameInfo.nFrameLenEx;
                m_lastFrameBuf = (unsigned char*)malloc(m_lastFrameLen);
                if (m_lastFrameBuf) {
                    memcpy(m_lastFrameBuf, stFrame.pBufAddr, m_lastFrameLen);
                    m_lastFrameInfo = stFrame.stFrameInfo;
                } else {
                    m_lastFrameLen = 0;
                }
            }
            m_camera->FreeImageBuffer(&stFrame);
        } else {
            msleep(10);
        }
    }
}

bool CameraThread::SaveLastFrameToFile(const QString& filePath, int type)
{
    QMutexLocker locker(&m_frameMutex);
    if (!m_lastFrameBuf || m_lastFrameLen == 0) return false;
    MV_CC_IMAGE stImage;
    memset(&stImage, 0, sizeof(MV_CC_IMAGE));
    stImage.nWidth = m_lastFrameInfo.nExtendWidth;
    stImage.nHeight = m_lastFrameInfo.nExtendHeight;
    stImage.enPixelType = m_lastFrameInfo.enPixelType;
    stImage.pImageBuf = m_lastFrameBuf;
    stImage.nImageLen = m_lastFrameInfo.nFrameLenEx;
    MV_CC_SAVE_IMAGE_PARAM saveParam;
    memset(&saveParam, 0, sizeof(saveParam));
    saveParam.enImageType = (MV_SAVE_IAMGE_TYPE)type;//0:未定义的图像格式 1:BMP图像格式 2:JPEG图像格式 3:PNG图像格式 4:TIFF图像格式
    saveParam.nQuality = 90;//JPG编码质量（50-99）
    saveParam.iMethodValue = 3;//插值方法 0-快速 1-均衡（其他值默认为均衡）2-最优 3-最优+
    QByteArray pathArr = filePath.toLocal8Bit();
    int ret = m_camera->SaveImageToFile(&stImage, &saveParam, pathArr.constData());
    return ret == 0;
}

bool CameraThread::RotateImageToUI(int rotate)
{
    MV_CC_ROTATE_IMAGE_PARAM _rotate;
//    _rotate.nWidth =
//    _rotate.nHeiht =
//    _rotate.nDstBufSize =
    _rotate.enRotationAngle =(MV_IMG_ROTATION_ANGLE)rotate;//旋转角度 1:90 2:180 3:270
    int ret = m_camera->RotateImage(&_rotate);
    return ret == 0;
}

