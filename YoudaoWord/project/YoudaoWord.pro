#-------------------------------------------------
#
# Project created by QtCreator 2014-10-06T17:17:23
#
#-------------------------------------------------

QT       += core gui xml sql network axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = YoudaoWord
INCLUDEPATH += .

CONFIG += static

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
RESOURCES += $$PWD/../resources/YoudaoWord.qrc
RC_FILE += 	$$PWD/../resources/YoudaoWord.rc
DEFINES += _CRT_SECURE_NO_WARNINGS QT

# Header file
HEADERS  += $$PWD/../src/widget.h \
			$$PWD/../src/YoudaoSetting.h \
			$$PWD/../src/ImportWidget.h \
			$$PWD/../src/ExportWidget.h \
			$$PWD/../src/YoudaoDatabase.h \
			$$PWD/../src/PrioritySettingDialog.h \
			$$PWD/../src/WordWidget.h \
			$$PWD/../src/DeviceWidget.h \
			$$PWD/../src/Communication.h \
			$$PWD/../src/bookInfo.h \
			$$PWD/../src/typedef.h \
			$$PWD/../src/BrowseDialog.h \
			$$PWD/../src/WordEditDialog.h \
			$$PWD/../src/MiscWidget.h \
			$$PWD/../src/WordCaptureDialog.h \
			$$PWD/../src/waitDialog.h \
			$$PWD/../src/backgroundWidget.h \
			
			
SOURCES += $$PWD/../src/main.cpp\
			$$PWD/../src/widget.cpp \
			$$PWD/../src/YoudaoSetting.cpp \
			$$PWD/../src/ImportWidget.cpp \
			$$PWD/../src/ExportWidget.cpp \
			$$PWD/../src/YoudaoDatabase.cpp	\
			$$PWD/../src/PrioritySettingDialog.cpp \
			$$PWD/../src/WordWidget.cpp \
			$$PWD/../src/DeviceWidget.cpp \
			$$PWD/../src/Communication.cpp \
			$$PWD/../src/bookInfo.cpp \
			$$PWD/../src/BrowseDialog.cpp \
			$$PWD/../src/WordEditDialog.cpp \
			$$PWD/../src/MiscWidget.cpp \
			$$PWD/../src/WordCaptureDialog.cpp \
			$$PWD/../src/waitDialog.cpp \
			$$PWD/../src/backgroundWidget.cpp \

TRANSLATIONS = $$PWD/../resources/Youdao_en.ts $$PWD/../resources/Youdao_zh.ts			