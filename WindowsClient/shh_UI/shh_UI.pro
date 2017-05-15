#-------------------------------------------------
#
# Project created by QtCreator 2017-03-22T18:47:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = shh_UI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    FileSystemWidget/filewidget.cpp \
    ShellTextEdit/shelltextedit.cpp \
    conwidegt.cpp

HEADERS  += mainwindow.h \
    FileSystemWidget/filewidget.h \
    ShellTextEdit/shelltextedit.h \
    conwidegt.h

FORMS    += mainwindow.ui \
    dialog.ui \
    feature.ui \
    filetransfer.ui \
    useloginauthentication.ui \
    new.ui

RESOURCES += \
    filetype.qrc
