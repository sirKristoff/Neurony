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


HEADERS += ../definitions.h \
	../net.h \
	../funs.h
