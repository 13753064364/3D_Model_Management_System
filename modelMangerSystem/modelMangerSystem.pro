QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    loginpage.cpp \
    forgotpasswordpage.cpp \
    storeselectionpage.cpp \
    homepage.cpp \
    networkmanager.cpp \
    configmanager.cpp

HEADERS += \
    mainwindow.h \
    loginpage.h \
    forgotpasswordpage.h \
    storeselectionpage.h \
    homepage.h \
    networkmanager.h \
    configmanager.h \
    3rd/include/CameraParams.h \
    3rd/include/MvCameraControl.h \
    3rd/include/MvErrorDefine.h \
    3rd/include/MvISPErrorDefine.h \
    3rd/include/MvObsoleteInterfaces.h \
    3rd/include/MvSdkExport.h \
    3rd/include/ObsoleteCamParams.h \
    3rd/include/PixelType.h

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
