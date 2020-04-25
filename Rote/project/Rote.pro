#-------------------------------------------------
#
# Project created by QtCreator 2015-07-20T13:35:58
#
#-------------------------------------------------

QT       += core gui xml sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = Rote
INCLUDEPATH += .

CONFIG += mobility
MOBILITY = 

#Arrange output folders
CONFIG(debug, debug|release){
    BUILDDIR = debug
} else {
    BUILDDIR = release
}
OBJECTS_DIR = $$PWD/../out/$$BUILDDIR/obj
UI_DIR = $$PWD/../out/$$BUILDDIR/ui
MOC_DIR = $$PWD/../out/$$BUILDDIR/moc
RCC_DIR = $$PWD/../out/$$BUILDDIR/rcc
DESTDIR = $$PWD/../out/$$BUILDDIR/bin
INCLUDEPATH += $$PWD/../headers
RESOURCES += $$PWD/../resources/Rote.qrc
RC_FILE += 	$$PWD/../resources/Rote.rc
DEFINES += _CRT_SECURE_NO_WARNINGS QT

# Header file
HEADERS  += $$PWD/../src/mainwindow.h \
    $$PWD/../src/infowidget.h \
    $$PWD/../src/rotewidget.h \
    $$PWD/../src/checkbutton.h \
    $$PWD/../src/youdaosetting.h \
    $$PWD/../src/YoudaoDatabase.h \
    $$PWD/../src/bookInfo.h \
    $$PWD/../src/typedef.h \
    $$PWD/../src/settingWidget.h \
    $$PWD/../src/BrowseWidget.h \
    $$PWD/../src/TextView.h \
    $$PWD/../src/NumberButton.h \
	

SOURCES += $$PWD/../src/main.cpp\
    $$PWD/../src/mainwindow.cpp \
    $$PWD/../src/infowidget.cpp \
    $$PWD/../src/rotewidget.cpp \
    $$PWD/../src/checkbutton.cpp \
    $$PWD/../src/youdaosetting.cpp \
    $$PWD/../src/YoudaoDatabase.cpp	\
    $$PWD/../src/settingWidget.cpp \
    $$PWD/../src/BrowseWidget.cpp \
    $$PWD/../src/bookInfo.cpp \
    $$PWD/../src/TextView.cpp \
    $$PWD/../src/NumberButton.cpp \

TRANSLATIONS = $$PWD/../resources/Rote_en.ts $$PWD/../resources/Rote_zh.ts			

ios {
HEADERS += $$PWD/../src/ios/AutoLock.h

OBJECTIVE_SOURCES += \
    $$PWD/../src/ios/AutoLock.mm \
}
