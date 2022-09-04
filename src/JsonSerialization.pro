TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += SUPPORT_GLM_SERIALIZATION

INCLUDEPATH += $$PWD/ext

SOURCES += \
        main.cpp
