#-------------------------------------------------
#
# Project created by QtCreator 2019-08-28T15:02:04
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = week2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        clientdialog.cpp \
        enterwindow.cpp \
        grid.cpp \
        main.cpp \
        mainwindow.cpp \
        manager.cpp \
        network.cpp \
        pawndialog.cpp \
        piece.cpp \
        serverdialog.cpp

HEADERS += \
        clientdialog.h \
        enterwindow.h \
        grid.h \
        mainwindow.h \
        manager.h \
        network.h \
        pawndialog.h \
        piece.h \
        serverdialog.h

FORMS += \
        clientdialog.ui \
        enterwindow.ui \
        mainwindow.ui \
        pawndialog.ui \
        serverdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
