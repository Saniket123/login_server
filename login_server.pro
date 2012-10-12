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
    face_recognition/face recognition.cpp \
    face_recognition/AffineTrans.cpp

HEADERS += \
    face_recognition.h

SOURCES += \
    face_recognition.cpp
