#-------------------------------------------------
#
# Company: EasySoft
# Author: Lunev Eduard
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OrCADComList
TEMPLATE = app

SOURCES += main.cpp\
    view/mainwindow.cpp \
    service/ParseComponentFile.cpp \
    service/ReferenceComponents.cpp \
    service/StrategyRegExp.cpp

HEADERS  += \
    service/ParseComponentFile.h \
    service/ReferenceComponents.h \
    service/StrategyRegExp.h \
    CONST.h \
    view/mainwindow.h

FORMS    += \
    view/mainwindow.ui

RC_ICONS = res/icon.ico
