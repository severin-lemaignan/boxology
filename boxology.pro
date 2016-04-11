#-------------------------------------------------
#
# Project created by QtCreator 2016-04-11T10:44:50
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = boxology
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

_BOOST_ROOT = $$BOOST_ROOT
isEmpty(_BOOST_ROOT) {
    message(\"Boost Library\" environment variable not detected...)
    !build_pass:error("Please set the environment variable `BOOST_ROOT`. For example, BOOST_ROOT=c:\\boost_1_53_0")
} else {
    message(\"Boost Library\" detected in BOOST_ROOT = \"$$_BOOST_ROOT\")
    INCLUDEPATH += $$_BOOST_ROOT
}

SOURCES += \
    src/app/main.cpp \
    src/app/mainwindow.cpp \
    src/view/cogbutton.cpp \
    src/view/edge.cpp \
    src/view/editablelabel.cpp \
    src/view/graphicsnode.cpp \
    src/view/scene.cpp \
    src/view/socket.cpp \
    src/view/tinybutton.cpp \
    src/view/view.cpp \
    src/architecture.cpp \
    src/cognitive_function.cpp \
    src/connection.cpp \
    src/json_visitor.cpp \
    src/jsoncpp.cpp \
    src/node.cpp \
    src/visitor.cpp \
    src/architecture_manager.cpp \
    src/tikz_visitor.cpp

HEADERS  += \
    src/app/mainwindow.hpp \
    src/json/json.h \
    src/view/cogbutton.hpp \
    src/view/edge.hpp \
    src/view/editablelabel.hpp \
    src/view/graphicsnode.hpp \
    src/view/graphicsnodedefs.hpp \
    src/view/scene.hpp \
    src/view/socket.hpp \
    src/view/tinybutton.hpp \
    src/view/view.hpp \
    src/architecture.hpp \
    src/cognitive_function.hpp \
    src/connection.hpp \
    src/json_visitor.hpp \
    src/node.hpp \
    src/visitor.hpp \
    src/architecture_manager.hpp \
    src/tikz_visitor.hpp

FORMS    += \
    src/app/mainwindow.ui

RESOURCES += \
    rc/resources.qrc
