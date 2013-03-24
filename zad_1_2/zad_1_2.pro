TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, debug|release) {
	DEFINES += DEBUG
}

SOURCES += main.cpp \
	../library/net/net.cpp \
	../library/net/funs.cpp


HEADERS += ../zad_2_0/definitions.h \
	../library/net/net.h \
	../library/net/funs.h
