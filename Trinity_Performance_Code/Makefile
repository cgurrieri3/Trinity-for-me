CCC = g++       # compiler
AR = ar         # archiver
LD = g++        # linker


ARFLAGS = rcs

LDFLAGS += -Wl,--no-as-needed -fPIC -ggdb3 -O3 -fno-omit-frame-pointer
LDFLAGS += $(shell root-config --libs)

CXXFLAGS += -c -fPIC  -ggdb3 -O3 
CXXFLAGS += $(shell root-config --cflags)

SRCFILES = $(wildcard *.cc)
OBJECTS = $(patsubst %.cc, %.o, ${SRCFILES})


ALLFLAGS = $(CXXFLAGS) -Wall

OutPutOpt     = -o



# rule for any compiling any .cpp file
%.o : %.cpp
	@printf "Compiling $< ... "
	@g++ $(ALLFLAGS) -c $<
	@echo "Done"


EXE = TrinityPerformanceCalculationsNew

all: ${EXE}

TrinityPerformanceCalculationsNew: TrinitySimUtilities.o TrinityPerformanceCalculationsNew.o
	$(LD)  $(LDFLAGS) $^ $(OutPutOpt) $@


clean:
	@rm -f *.o *.a *.so *~ core $(EXE)

.SUFFIXES: .o


depend:
	$(DEPEND)

Makefile.depend:
	$(DEPEND)

DEPEND=echo > Makefile.depend0 &&\
	makedepend -s "\#DEPEND LIST DONT DELETE" -- $(INCLUDEFLAGS) \
		-Y --  *.cpp *.c \
		-f Makefile.depend0 > /dev/null 2>&1 &&\
	sed "s/^[a-zA-Z0-9]*\///" Makefile.depend0 > Makefile.depend &&\
	rm -f Makefile.depend0

include Makefile.depend

