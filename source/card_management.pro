#-------------------------------------------------
#
# Project created by QtCreator 2022-02-21T11:42:39
#
#-------------------------------------------------
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = card_management
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    student.cpp \
    manager.cpp \
    card.cpp \
    log.cpp \
    canteen.cpp \
    message.cpp \
    window.cpp

HEADERS  += mainwindow.h \
    student.h \
    manager.h \
    card.h \
    log.h \
    canteen.h \
    message.h \
    window.h

FORMS    += mainwindow.ui
