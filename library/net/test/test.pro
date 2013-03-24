TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, debug|release) {
	DEFINES += DEBUG
}

SOURCES += main.cpp \
	../net.cpp \
	../funs.cpp


HEADERS += ../zad_2_0/definitions.h \
	../net.h \
	../funs.h
