#include "camera.h"
#include <QDebug>
Camera::Camera() : m_handle(nullptr)
{
    MV_CC_Initialize();    
}

Camera::~Camera() {
    MV_CC_Finalize();
    CloseDevice();
    DestroyHandle();
}

int Camera::InitSDK() {
    return MV_CC_Initialize();
}
int Camera::EnumDevices(unsigned int nTLayerType, MV_CC_DEVICE_INFO_LIST* pstDevList) {
    return MV_CC_EnumDevices(nTLayerType, pstDevList);
}
int Camera::CreateHandle(MV_CC_DEVICE_INFO* pstDeviceInfo) {
    if (m_handle) {
        DestroyHandle();
    }
    return MV_CC_CreateHandle(&m_handle, pstDeviceInfo);
}
int Camera::OpenDevice() {
    if (!m_handle) return -1;

    return MV_CC_OpenDevice(m_handle, MV_ACCESS_Exclusive, 0);
}
int Camera::CloseDevice() {
    if (m_handle) {
        return MV_CC_CloseDevice(m_handle);
    }

    int ret = DestroyHandle();
    m_handle = nullptr;
    return ret;
}
int Camera::DestroyHandle() {
    if (m_handle) {
        int ret = MV_CC_DestroyHandle(m_handle);
        m_handle = nullptr;
        return ret;
    }
    return 0;
}
bool Camera::IsDeviceConnected() {
    if (m_handle) {
        return MV_CC_IsDeviceConnected(m_handle);
    }
    return false;
}
int Camera::StartGrabbing() {
    MV_CC_SetImageNodeNum(m_handle,5);
    if (m_handle) {
        return MV_CC_StartGrabbing(m_handle);
    }
    return -1;
}
int Camera::StopGrabbing() {
    if (m_handle) {
        return MV_CC_StopGrabbing(m_handle);
    }
    return -1;
}
int Camera::GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec) {
    if (m_handle) {
        return MV_CC_GetImageBuffer(m_handle, pFrame, nMsec);
    }
    return -1;
}
int Camera::FreeImageBuffer(MV_FRAME_OUT* pFrame) {
    if (m_handle) {
        return MV_CC_FreeImageBuffer(m_handle, pFrame);
    }
    return -1;
}
int Camera::DisplayOneFrame(void* hDisplay, MV_CC_IMAGE* pImageInfo) {
    if (m_handle) {
        return MV_CC_DisplayOneFrameEx2(m_handle, hDisplay, pImageInfo, 0);
    }
    return -1;
}

int Camera::SetBayerCvtQuality(unsigned int nQuality) {
    if (m_handle) {
        return MV_CC_SetBayerCvtQuality(m_handle, nQuality);
    }
    return -1;
}

int Camera::ConvertPixelType(MV_CC_PIXEL_CONVERT_PARAM* pstCvtParam) {
    if (m_handle) {
        int convertRet = MV_CC_ConvertPixelType(m_handle, pstCvtParam);
//        qDebug() << "[CameraThread] ConvertPixelType ret=" << convertRet;
        return convertRet;
    }
    return -1;
}

int Camera::GetIntValue(const char* strKey, MVCC_INTVALUE* pIntValue)
{
    if (m_handle) {
        return MV_CC_GetIntValue(m_handle, strKey, pIntValue);
    }
    return -1;
}

int Camera::SetIntValue(const char* strKey, unsigned int nValue)
{
    if (m_handle) {
        return MV_CC_SetIntValue(m_handle, strKey, nValue);
    }
    return -1;
}

int Camera::GetFloatValue(const char* strKey, MVCC_FLOATVALUE* pFloatValue)
{
    if (m_handle) {
        return MV_CC_GetFloatValue(m_handle, strKey, pFloatValue);
    }
    return -1;
}

int Camera::SetFloatValue(const char* strKey, float fValue)
{
    if (m_handle) {
        return MV_CC_SetFloatValue(m_handle, strKey, fValue);
    }
    return -1;
}

int Camera::GetBoolValue(const char* strKey, bool* pbValue)
{
    if (m_handle) {
        return MV_CC_GetBoolValue(m_handle, strKey, pbValue);
    }
    return -1;
}

int Camera::SetBoolValue(const char* strKey, bool bValue)
{
    if (m_handle) {
        return MV_CC_SetBoolValue(m_handle, strKey, bValue);
    }
    return -1;
}

int Camera::GetStringValue(const char* strKey, MVCC_STRINGVALUE* pStringValue)
{
    if (m_handle) {
        return MV_CC_GetStringValue(m_handle, strKey, pStringValue);
    }
    return -1;
}

int Camera::SetStringValue(const char* strKey, const char* strValue)
{
    if (m_handle) {
        return MV_CC_SetStringValue(m_handle, strKey, strValue);
    }
    return -1;
}
int Camera::SetEnumValue(const char* key, unsigned int value) {
    if (m_handle) {
        return MV_CC_SetEnumValue(m_handle, key, value);
    }
    return -1;
}
int Camera::GetEnumValue(const char* strKey, MVCC_ENUMVALUE* pEnumValue)
{
    if (m_handle) {
        return MV_CC_GetEnumValue(m_handle, strKey, pEnumValue);
    }
    return -1;
}
int Camera::SaveImageToFile(MV_CC_IMAGE* pImage, MV_CC_SAVE_IMAGE_PARAM* pSaveImageParam, const char* pcImagePath) {
    if (m_handle) {
        return MV_CC_SaveImageToFileEx2(m_handle, pImage, pSaveImageParam, pcImagePath);
    }
    return -1;
}
int Camera::RotateImage(MV_CC_ROTATE_IMAGE_PARAM* pstRotateParam)
{
    if (m_handle) {
        return MV_CC_RotateImage(m_handle, pstRotateParam);
    }
    return -1;
}

int Camera::SetEnumValueByString(const char* key, const char* sValue) {
    if (m_handle) {
        return MV_CC_SetEnumValueByString(m_handle, key, sValue);
    }
    return -1;
}
