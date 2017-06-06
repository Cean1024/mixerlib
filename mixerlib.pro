TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lasound -lpthread

SOURCES += main.cpp \
    alsaapi.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    alsaapi.h \
    globaflags.h

