#-------------------------------------------------
#
# Project created by QtCreator 2016-02-13T10:16:42
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fastfoodfantasy
TEMPLATE = app

INCLUDEPATH += $$PWD/../firmware/SealHAT

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
    config_pages/xcel_config.cpp \
    src/usb_sending_serialization.cpp \
    src/usb_serial_port_setup.cpp \
    src/sealhat_device.cpp \
    src/data_streaming.cpp \
    src/data_retreival.cpp \
    src/sensorsample.cpp \
    src/crc32.cpp

FORMS    += \
    maindialog.ui


HEADERS  += \
    maindialog.h \
    analyze.h \
    ../firmware/SealHAT/seal_Types.h \
    src/sealhat_device.h \
    src/sensorsample.h \
    src/crc32.h

RESOURCES += \
    images.qrc
