#-------------------------------------------------
#
# Project created by QtCreator 2012-10-11T10:00:22
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = login_server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

HEADERS += \
    tcp_receiver.h

SOURCES += \
    tcp_receiver.cpp

OTHER_FILES += \
    face_recognition/PCAModel.bin \
    face_recognition/GalleryPeopleNum.ini \
    face_recognition/GalleryPCACoeff.bin

HEADERS += \
    face_recognition/MemoryOper.h \
    face_recognition/MatrixOper.h \
    face_recognition/BasicDataType.h \
    face_recognition/AffineTrans.h

SOURCES += \
    face_recognition/MatrixOper.cpp \
    face_recognition/AffineTrans.cpp

HEADERS += \
    face_recognition.h

SOURCES += \
    face_recognition.cpp

HEADERS += \
    pcamodel.h

SOURCES += \
    pcamodel.cpp

HEADERS += \
    login_server.h

SOURCES += \
    login_server.cpp

unix{
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

win32{
    INCLUDEPATH += "C:/opencv/opencv/build/include/opencv" \
                   "C:/opencv/opencv/build/include"

    LIBS += "C:/opencv/lib/libopencv_core241.dll.a" \
            "C:/opencv/lib/libopencv_features2d241.dll.a" \
            "C:/opencv/lib/libopencv_highgui241.dll.a" \
            "C:/opencv/lib/libopencv_video241.dll.a" \
            "C:/opencv/lib/libopencv_imgproc241.dll.a"
}
