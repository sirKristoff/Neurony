TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, debug|release) {
	DEFINES += DEBUG
	DEFINES += LVL3
}

SOURCES += main.cpp \
	../library/net/net.cpp \
	../library/net/funs.cpp

HEADERS += ../library/getopt/getopt.h \
	../library/net/definitions.h \
	../library/net/net.h \
	../library/net/funs.h
