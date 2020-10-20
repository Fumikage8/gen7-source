# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = lib
DESTDIR = ../../lib/mingw
TARGET = ExFrameworkd
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += staticlib debug 
QT = core gui opengl
SOURCES += _UnityFramework.cpp
HEADERS += ExZip.h ExPad.h ExCamera.h ExRender.h ExFramework.h ExApplication.h ExQt.h ExGLWindow.h ExGLWidget.h
FORMS +=
RESOURCES +=
TRANSLATIONS +=
OBJECTS_DIR = build/Debug/MinGW-Windows
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc.exe
QMAKE_CXX = g++.exe
DEFINES += EX_DEBUG 
INCLUDEPATH += ../../../2010.04/qt/src/3rdparty/zlib ../../../2010.04/qt/include/Qt ../../opensource/GL/mingw 
LIBS += 
