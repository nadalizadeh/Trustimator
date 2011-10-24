#-------------------------------------------------
#
# Project created by QtCreator 2011-09-18T14:26:18
#
#-------------------------------------------------

QT       += core gui

TARGET = Trustimator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    QJson/serializerrunnable.cpp \
    QJson/serializer.cpp \
    QJson/qobjecthelper.cpp \
    QJson/parserrunnable.cpp \
    QJson/parser.cpp \
    QJson/json_scanner.cpp \
    QJson/json_parser.cc \
    dataset.cpp

HEADERS  += mainwindow.h \
    QJson/stack.hh \
    QJson/serializerrunnable.h \
    QJson/serializer.h \
    QJson/qobjecthelper.h \
    QJson/qjson_export.h \
    QJson/qjson_debug.h \
    QJson/position.hh \
    QJson/parserrunnable.h \
    QJson/parser.h \
    QJson/parser_p.h \
    QJson/location.hh \
    QJson/json_scanner.h \
    QJson/json_parser.hh \
    dataset.h

FORMS    += mainwindow.ui
