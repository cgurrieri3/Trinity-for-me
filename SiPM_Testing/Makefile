########################################################
#
#  Makefile for drsosc, drscl and drs_exam 
#  executables under linux
#
#  Requires wxWidgets 2.8.9 or newer
#
########################################################

# check if wxWidgets is installed
HAVE_WX       = $(shell which wx-config)
ifeq ($(HAVE_WX),)
$(error Error: wxWidgets required to compile "drsosc")
endif

# check for OS
OS            = $(shell uname)
ifeq ($(OS),Darwin)
DOS           = OS_DARWIN
else
DOS           = OS_LINUX
endif

CFLAGS        = -g -fno-strict-aliasing -Iinclude -I/usr/local/include -D$(DOS) -DHAVE_USB -DHAVE_LIBUSB10 -DUSE_DRS_MUTEX
LIBS          = -lpthread -lutil -lusb-1.0

ifeq ($(OS),Darwin)
CFLAGS        += -stdlib=libstdc++
endif         

# Root

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
ROOTLDFLAGS  := $(shell root-config --ldflags)
CLLFLAGS += $(ROOTLDFLAGS)

CFLAGS     += $(ROOTCFLAGS)
LIBS       += $(ROOTGLIBS)



# wxWidgets libs and flags
WXLIBS        = $(shell wx-config --libs)
WXFLAGS       = $(shell wx-config --cxxflags)

CPP_OBJ       = DRS.o averager.o
OBJECTS       = musbstd.o mxml.o 
MYOBJECTS     = DRSDigitizer.o
OBJS 			  = StageControl.o SwitchControl.o SignalAnalysis.o

all: ModuleScan tester 

ModuleScan: $(MYOBJECTS) $(OBJECTS) $(CPP_OBJ) $(OBJS) Scan.o
	$(CXX) $(CFLAGS) -o ModuleScan Scan.o $(OBJS) $(OBJECTS) $(MYOBJECTS) $(CPP_OBJ) $(LIBS) $(WXLIBS)

SwitchChannel: $(MYOBJECTS) $(OBJECTS) $(CPP_OBJ) $(OBJS) SwitchChannel.o
	$(CXX) $(CFLAGS) -o SwitchChannel SwitchChannel.o $(OBJS) $(OBJECTS) $(MYOBJECTS) $(CPP_OBJ) $(LIBS) $(WXLIBS)

tester: $(MYOBJECTS) $(OBJECTS) $(CPP_OBJ) $(OBJS) tester.o
	$(CXX) $(CFLAGS) -o tester tester.o $(OBJS) $(OBJECTS) $(MYOBJECTS) $(CPP_OBJ) $(LIBS) $(WXLIBS)

SwitchChannel.o: SwitchChannel.cpp SignalAnalysis.h StageControl.h SwitchControl.h
	$(CXX) $(CFLAGS) -c SwitchChannel.cpp $(LIBS)

Scan.o: Scan.cpp SignalAnalysis.h StageControl.h SwitchControl.h
	$(CXX) $(CFLAGS) -c Scan.cpp $(LIBS)

tester.o: tester.cpp SignalAnalysis.h StageControl.h
	$(CXX) $(CFLAGS) -c tester.cpp $(LIBS)

StageControl.o: StageControl.cpp StageControl.h 
	$(CXX) $(CFLAGS) -c StageControl.cpp $(LIBS)

SwitchControl.o: SwitchControl.cpp SwitchControl.h
	$(CXX) $(CFLAGS) -c SwitchControl.cpp $(LIBS)

SignalAnalysis.o: SignalAnalysis.cpp SignalAnalysis.h
	$(CXX) $(CFLAGS) -c SignalAnalysis.cpp $(LIBS)

DRS.o: DRS.cpp DRS.h mxml.h averager.h musbstd.h
	$(CXX) $(CFLAGS) $(WXFLAGS) -c DRS.cpp $(LIBS)

DRSDigitizer.o: DRSDigitizer.cpp DRSDigitizer.h DRS.h
	$(CXX) $(CFLAGS) -c DRSDigitizer.cpp $(LIBS)

averager.o: averager.cpp averager.h
	$(CXX) $(CFLAGS) $(WXFLAGS) -c averager.cpp $(LIBS)

musbstd.o: musbstd.c musbstd.h
	$(CXX) $(CFLAGS) -c musbstd.c $(LIBS)

mxml.o: mxml.c mxml.h
	$(CXX) $(CFLAGS) -c mxml.c $(LIBS)


clean:
	rm -f *.o ModuleScan

