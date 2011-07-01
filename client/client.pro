#-------------------------------------------------
#
# Project created by QtCreator 2011-07-01T12:58:44
#
#-------------------------------------------------

QT       += core gui

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../proto/netraw.cpp \
    ../proto/grSim_Replacement.pb.cc \
    ../proto/grSim_Commands.pb.cc \
    ../proto/grSim_Packet.pb.cc

HEADERS  += mainwindow.h
INCLUDEPATH += ../include
LIBS += -L../libs/ -lprotobuf-2.4.0
