# -------------------------------------------------
# Project created by QtCreator 2009-11-18T18:56:10
# -------------------------------------------------
QT += network \
    opengl
TARGET = Simulator
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    widget.cpp \
    Graphics/graphics.cpp \
    Physics/pworld.cpp \
    Physics/pobject.cpp \
    Physics/pball.cpp \
    Physics/pground.cpp \
    sslworld.cpp \
    Physics/pfixedbox.cpp \
    Physics/pcylinder.cpp \
    robot.cpp \
    Physics/pbox.cpp \
    proto/netraw.cpp \
    proto/robocup_ssl_server.cpp \
    proto/messages_robocup_ssl_wrapper.pb.cc \
    proto/messages_robocup_ssl_refbox_log.pb.cc \
    proto/messages_robocup_ssl_geometry.pb.cc \
    proto/messages_robocup_ssl_detection.pb.cc \
    Physics/pray.cpp
HEADERS += mainwindow.h \
    widget.h \
    Graphics/graphics.h \
    Physics/pworld.h \
    Physics/pobject.h \
    Physics/pball.h \
    Physics/pground.h \
    sslworld.h \
    Physics/pfixedbox.h \
    Physics/pcylinder.h \
    robot.h \
    Physics/pbox.h \
    config.h \
    proto/netraw.h \
    proto/robocup_ssl_server.h \
    proto/messages_robocup_ssl_refbox_log.pb.h \
    proto/messages_robocup_ssl_geometry.pb.h \
    proto/messages_robocup_ssl_detection.pb.h \
    proto/messages_robocup_ssl_wrapper.pb.h \
    Physics/pray.h
win32:LIBS += -L$$PWD/libs/win32 \
    -lode-0.11.1 \
    -lprotobuf-2.1.0 \
    -lws2_32
unix:LIBS += -L$$PWD/libs/unix \
    -lode-0.11.1 \
    -lprotobuf-2.1.0
unix:OBJECTS_DIR = $$PWD/objs
INCLUDEPATH += $$PWD/include
RESOURCES += textures.qrc
