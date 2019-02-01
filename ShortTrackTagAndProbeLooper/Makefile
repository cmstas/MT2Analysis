#
# stuff to make
#
ifndef ROOTSYS
all:
	@echo "ROOTSYS is not set. Please set ROOT environment properly"; echo
else

all: 	build
help:
	@echo "Available Targets:";\
	cat Makefile | perl -ne 'printf("\t%-15s %s\n",$$1,$$2) if(/^(\S+):[^#]+(#.*)$$/)'

ifndef VERBOSE
  QUIET := @
endif

NAME=ShortTrackTagAndProbeLooper

CC = g++ 
CMSROOT = ..
COREDIR = $(CMSROOT)/CORE
MT2DIR = $(CMSROOT)/MT2CORE
INCLUDE = $(shell root-config --cflags) -I$(COREDIR) -I$(MT2DIR)
CFLAGS = -Wall -Wno-unused-function -g -O2 -fPIC $(INCLUDE)
ROOTLIBS = $(shell root-config --ldflags --cflags --libs) -lTMVA #-lEG -lGenVector

DICTINCLUDE = $(ROOTSYS)/include/Math/QuantFuncMathCore.h $(ROOTSYS)/include/TLorentzVector.h $(ROOTSYS)/include/Math/Vector4D.h

LINKER = g++
LINKERFLAGS = $(shell root-config --ldflags --libs) -lEG -lGenVector -lTMVA -lRooFit

SOURCES = $(wildcard *.cc)
OBJECTS = $(SOURCES:.cc=.o)
LIB = lib$(NAME).so

CORESOURCES = $(wildcard $(COREDIR)/*.cc) $(wildcard $(COREDIR)/Tools/*.cc) $(wildcard $(COREDIR)/Tools/MT2/*.cc)  $(wildcard $(COREDIR)/Tools/btagsf/*.cc)  $(wildcard $(COREDIR)/Tools/dorky/*.cc)
COREOBJECTS = $(CORESOURCES:.cc=.o) 
CORELIB = lib$(NAME)CORE.so

MT2CORESOURCES = $(wildcard $(MT2DIR)/*.cc) 
MT2COREOBJECTS = $(MT2CORESOURCES:.cc=.o) 
MT2CORELIB = lib$(NAME)MT2CORE.so

DICT = LinkDef_out.o

LIBS = $(LIB) $(CORELIB) $(MT2CORELIB) 

EXE = $(NAME).exe

#
# how to make it
#

$(CORELIB): $(DICT) $(COREOBJECTS)
	$(QUIET) echo "Linking $@"; \
	echo "$(LINKER) -shared -o $@ $(COREOBJECTS) $(DICT) $(LINKERFLAGS)"; \
	$(LINKER) -shared -o $@ $(COREOBJECTS) $(DICT) $(LINKERFLAGS)

$(MT2CORELIB): $(DICT) $(MT2COREOBJECTS)
	$(QUIET) echo "Linking $@"; \
	echo "$(LINKER) -shared -o $@ $(MT2COREOBJECTS) $(DICT) $(LINKERFLAGS)"; \
	$(LINKER) -shared -o $@ $(MT2COREOBJECTS) $(DICT) $(LINKERFLAGS)

$(LIB):	$(DICT) $(OBJECTS)
	$(QUIET) echo "Linking $@"; \
	echo "$(LINKER) -shared -o $@ $(OBJECTS) $(DICT) $(LINKERFLAGS)"; \
	$(LINKER) -shared -o $@ $(OBJECTS) $(DICT) $(LINKERFLAGS)

LinkDef_out.cxx: LinkDef.h
	$(QUIET) echo "Making CINT dictionaries"; \
	rootcint -f LinkDef_out.cc -c -p $(DICTINCLUDE)  LinkDef.h; \
	cat LinkDef_out.cc > LinkDef_out.cxx; rm LinkDef_out.cc

# the option "-Wl,-rpath,./" adds ./ to the runtime search path for libraries
$(EXE):  $(LIBS)
	$(QUIET) echo "Building $@"; \
	echo "$(CC) -o $@ $(LIBS) $(ROOTLIBS) -Wl,-rpath,./"; \
	$(CC) $(CFLAGS) -o $@ $(LIBS) $(ROOTLIBS) -Wl,-rpath,./ 

%.o: 	%.cc %.h
	$(QUIET) echo "Compiling $<"; \
	$(CC) $(CFLAGS) $< -c -o $@

%.o: 	%.C %.h
	$(QUIET) echo "Compiling $<"; \
	$(CC) $(CFLAGS) $< -c -o $@

%.o: 	%.cc
	$(QUIET) echo "Compiling $<"; \
	$(CC) $(CFLAGS) $< -c -o $@

%.o: 	%.C
	$(QUIET) echo "Compiling $<"; \
	$(CC) $(CFLAGS) $< -c -o $@

%.o:    %.cxx 
	$(QUIET) echo "Compiling $<"; \
	$(CC) $(CFLAGS) $< -c -o $@

libs:	$(LIBS)

build:  $(EXE)

b: build

clean:
	rm -f \
	LinkDef_out* \
	*.o \
	$(LIBS) \
	$(EXE) \
        ../CORE/*.o \
        ../CORE/*.d \
        ../CORE/*.so \
	../MT2Sources/*.o \
	../MT2Sources/*.d \
	../MT2Sources/*.so \
        ../CORE/Tools/*.o \
	../CORE/Tools/*.d \
        ../CORE/Tools/*.so \
        ../CORE/Tools/MT2/*.o \
	../CORE/Tools/MT2/*.d \
        ../CORE/Tools/MT2/*.so \
        ../CORE/Tools/btagsf/*.o \
	../CORE/Tools/btagsf/*.d \
        ../CORE/Tools/btagsf/*.so 

localclean:
	rm -f \
	LinkDef_out* \
	*.o \
	*.so \
	*.exe          
endif
