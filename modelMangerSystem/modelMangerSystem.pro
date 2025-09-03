QT       += core gui network serialport webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    loginpage.cpp \
    forgotpasswordpage.cpp \
    storeselectionpage.cpp \
    homepage.cpp \
    addcustomerpage.cpp \
    networkmanager.cpp \
    $$PWD/drivers/camera.cpp \
    $$PWD/drivers/ledcontroller.cpp \
    camerathread.cpp \
    widget.cpp \
    configmanager.cpp

HEADERS += \
    mainwindow.h \
    loginpage.h \
    forgotpasswordpage.h \
    storeselectionpage.h \
    homepage.h \
    addcustomerpage.h \
    networkmanager.h \
    configmanager.h \
    $$PWD/drivers/camera.h \
    $$PWD/drivers/ledcontroller.h \
    camerathread.h \
    widget.h \
    3rd/include/CameraParams.h \
    3rd/include/MvCameraControl.h \
    3rd/include/MvErrorDefine.h \
    3rd/include/MvISPErrorDefine.h \
    3rd/include/MvObsoleteInterfaces.h \
    3rd/include/MvSdkExport.h \
    3rd/include/ObsoleteCamParams.h \
    3rd/include/PixelType.h


FORMS += \
    widget.ui

RESOURCES += \
    resources.qrc


msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/3rd/HKSDK/include
LIBS += -L$$PWD/3rd/HKSDK/libs -lMvCameraControl
