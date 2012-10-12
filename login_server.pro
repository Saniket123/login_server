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
