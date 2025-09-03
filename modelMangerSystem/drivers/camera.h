#ifndef CAMERA_H
#define CAMERA_H

#include "MvCameraControl.h"
class Camera
{
public:
    Camera();
    ~Camera();
    int InitSDK();
    int EnumDevices(unsigned int nTLayerType, MV_CC_DEVICE_INFO_LIST* pstDevList);
    int CreateHandle(MV_CC_DEVICE_INFO* pstDeviceInfo);
    int OpenDevice();
    int CloseDevice();
    int DestroyHandle();
    bool IsDeviceConnected();
    int StartGrabbing();
    int StopGrabbing();
    int GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec);
    int FreeImageBuffer(MV_FRAME_OUT* pFrame);
    int DisplayOneFrame(void* hDisplay, MV_CC_IMAGE* pImageInfo);
    int SetBayerCvtQuality(unsigned int nQuality);
    int ConvertPixelType(MV_CC_PIXEL_CONVERT_PARAM* pstCvtParam);
    int GetIntValue(const char* strKey, MVCC_INTVALUE* pIntValue);
    int SetIntValue(const char* strKey, unsigned int nValue);
    int GetFloatValue(const char* strKey, MVCC_FLOATVALUE* pFloatValue);
    int SetFloatValue(const char* strKey, float fValue);
    int GetBoolValue(const char* strKey, bool* pbValue);
    int SetBoolValue(const char* strKey, bool bValue);
    int GetStringValue(const char* strKey, MVCC_STRINGVALUE* pStringValue);
    int SetStringValue(const char* strKey, const char* strValue);
    int GetEnumValue(const char* strKey, MVCC_ENUMVALUE* pEnumValue);
    int SetEnumValue(const char* key, unsigned int value);
    int SetEnumValueByString(const char* key, const char* sValue);
    int SaveImageToFile(MV_CC_IMAGE* pImage, MV_CC_SAVE_IMAGE_PARAM* pSaveImageParam, const char* pcImagePath);
    int RotateImage(MV_CC_ROTATE_IMAGE_PARAM* pstRotateParam);

private:
    void* m_handle;
};

#endif // CAMERA_H
