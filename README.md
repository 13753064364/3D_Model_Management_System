# 3D_Model_Management_System
3D Model Management System

## 开发进度

### 2025年9月2日 开发进度

#### 完成功能
1. **登录页面**
2. **主页页面**
3. **选择门店页面**、

### 2025年9月3日 开发进度

#### 完成功能
1. **网络api登录/获取用户信息调用**
2. **隐私协议页面**、

### 2025年9月6日 开发进度

#### 完成功能
1. **用户头像 选项列表**
2. **顾客信息列表优化**、

### 2025年9月7日 开发进度

#### 完成功能
1. **Jetson nano b01移植调试**
   win32 {
    LIBS += -L$$PWD/3rd/HKSDK/libs -lMvCameraControl
}

unix {
    LIBS += -L/opt/MVS/lib/aarch64/ \
            -lMVGigEVisionSDK \
            -lMvCameraControl \
            -lMVRender \
            -lFormatConversion \
            -lMediaProcess
}


#### 项目结构
```
modelMangerSystem/
├── 源代码文件 (.cpp/.h)
├── 资源文件 (images/)
├── 项目配置 (.pro)
