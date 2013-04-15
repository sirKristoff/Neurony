
#*.o #$(SOURCES:.cpp=.o)

CXX       = g++
DEFINES   = -DDEBUG
CFLAGS    = -c
LDFLAGS   =
RM        = rm -fv

OUTDIR      = build/
OBJECTS_DIR = $(OUTDIR)
TARGET      = $(OUTDIR)net

SOURCES = main.cpp \
	  ../net.cpp \
	  ../funs.cpp

OBJECTS = $(OBJECTS_DIR)main.o \
	  $(OBJECTS_DIR)net.o \
	  $(OBJECTS_DIR)funs.o


all : $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

$(OBJECTS_DIR)main.o : main.cpp
	$(CXX) $(CFLAGS) $<  -o $@

$(OBJECTS_DIR)net.o : ../net.cpp
	$(CXX) $(CFLAGS) $<  -o $@

$(OBJECTS_DIR)funs.o : ../funs.cpp
	$(CXX) $(CFLAGS) $<  -o $@

clean :
	$(RM) $(OBJECTS) $(TARGET)
