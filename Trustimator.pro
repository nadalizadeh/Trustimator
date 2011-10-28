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
    dataset.cpp \
    fann/src/fann.c \
    fann/src/fann_train.c \
    fann/src/fann_train_data.c \
    fann/src/fann_io.c \
    fann/src/fann_error.c \
    fann/src/fann_cascade.c \
    estimator_base.cpp \
    estimator_neuralnet1.cpp \
    estimator_neuralnet.cpp \
    estimator_neuralnetcascade.cpp \
    estimator_leaveoneout.cpp \
    single_estimate_dialog.cpp

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
    dataset.h \
    fann/include/floatfann.h \
    fann/include/fann.h \
    fann/include/fann_train.h \
    fann/include/fann_io.h \
    fann/include/fann_internal.h \
    fann/include/fann_error.h \
    fann/include/fann_data.h \
    fann/include/fann_cpp.h \
    fann/include/fann_cascade.h \
    fann/include/fann_activation.h \
    fann/include/config.h \
    fann/include/compat_time.h \
    estimator_base.h \
    estimator_neuralnet1.h \
    estimator_neuralnet.h \
    estimator_neuralnetcascade.h \
    estimator_leaveoneout.h \
    single_estimate_dialog.h

FORMS    += mainwindow.ui \
    single_estimate_dialog.ui
INCLUDEPATH += fann/include
