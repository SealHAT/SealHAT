#-------------------------------------------------
#
# Project created by QtCreator 2016-02-13T10:16:42
#
#-------------------------------------------------

QT       += core gui serialport
         sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fastfoodfantasy
TEMPLATE = app

INCLUDEPATH += $$PWD/../firmware/SealHAT
INCLUDEPATH += $$PWD/../firmware/SealHAT/lsm303
INCLUDEPATH += $$PWD/../firmware/SealHAT/si7050x
INCLUDEPATH += $$PWD/../firmware/SealHAT/max44009
INCLUDEPATH += $$PWD/../firmware/SealHAT/max30003
INCLUDEPATH += $$PWD/../firmware/SealHAT/sam-m8q

SOURCES += main.cpp\
    maindialog.cpp \
    analyze.cpp \
    loading_data.cpp \
    config_pages/configure.cpp \
    config_pages/configurelist_page.cpp \
    config_pages/ekg_config.cpp \
    config_pages/gps_config.cpp \
    config_pages/mag_config.cpp \
    config_pages/temp_config.cpp \
    serial/usb_receiving_serialization.cpp \
    serial/usb_sending_serialization.cpp \
    serial/usb_serial_port_setup.cpp \
    serial/data_retrieval.cpp \
    config_pages/xcel_config.cpp

FORMS    += \
    maindialog.ui


HEADERS  += \
    maindialog.h \
    analyze.h \
    ../firmware/SealHAT/seal_Types.h

RESOURCES += \
    images.qrc
