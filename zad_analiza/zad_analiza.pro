TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
	../library/net/net.cpp \
	../library/net/funs.cpp

HEADERS += ../library/net/definitions.h \
	../library/net/net.h \
	../library/net/funs.h

