CC = g++
INCLUDE = -I../ -I./ 
CFLAGS = -Wall -g -fPIC $(shell root-config --cflags) $(INCLUDE) $(EXTRACFLAGS) -DTOOLSLIB
LINKER = g++

LINKERFLAGS = $(shell root-config --ldflags) $(shell root-config --libs) -lMinuit
ifeq ($(shell root-config --platform),macosx)
	LINKERFLAGS = -dynamiclib -undefined dynamic_lookup -Wl,-x -O -Xlinker -bind_at_load -flat_namespace $(shell root-config --libs) -lEG -lGenVector
endif

SOURCES =  GammaID.cc 
OBJECTS = $(SOURCES:.cc=.o) LinkDef_out.o
LIB = libGammaID.so

CORESOURCES = $(wildcard ../MT2CORE/*.cc) $(wildcard ../MT2CORE/MT2/*.cc) $(wildcard ../MT2CORE/Plotting/*.cc)
COREOBJECTS = $(CORESOURCES:.cc=.o) 
CORELIB = libGammaIDCORE.so

LIBS = $(LIB) $(CORELIB) 

.PHONY: all help compile clean cms2env

libs:	$(LIBS)

$(LIB):	$(OBJECTS) 
	echo "Linking $(LIB)"; \
	$(LINKER) $(LINKERFLAGS) -shared $(OBJECTS) -o $@

$(CORELIB):	$(COREOBJECTS) 
	echo "Linking $(CORELIB)"; \
	$(LINKER) $(LINKERFLAGS) -shared $(COREOBJECTS) -o $@

LinkDef_out.cxx: LinkDef.h GammaID.h 
	rootcint -f $@ -c $(INCLUDE) GammaID.h  $<

# General rule for making object files
%.d:	%.cc
	$(CC) -MM -MT $@ -MT ${@:.d=.o} $(CFLAGS) $< > $@; \
                     [ -s $@ ] || rm -f $@
%.d:	%.cxx
	$(CC) -MM -MT $@ -MT ${@:.d=.o} $(CFLAGS) $< > $@; \
                     [ -s $@ ] || rm -f $@

%.o: 	%.cc 
	$(CC) $(CFLAGS) $< -c -o $@

%.o: 	%.cxx
	$(CC) $(CFLAGS) $< -c -o $@

.PHONY: all
all:	$(LIBS)  

.PHONY: clean
clean:  
	rm -v -f \
        ../MT2CORE/*.o \
        ../MT2CORE/*.d \
        ../MT2CORE/*.so \
	../MT2CORE/MT2/*.o \
	../MT2CORE/MT2/*.d \
	../MT2CORE/MT2/*.so \
	../MT2CORE/Plotting/*.o \
	../MT2CORE/Plotting/*.d \
	../MT2CORE/Plotting/*.so \
        $(CORELIB) $(LIB) \
        LinkDef_out.cxx  LinkDef_out.h \
        *.o *.d *.so; echo "Done"

test: 
	@echo $(INCLUDE)

-include $(LIBDIR)/LinkDef_out.d

