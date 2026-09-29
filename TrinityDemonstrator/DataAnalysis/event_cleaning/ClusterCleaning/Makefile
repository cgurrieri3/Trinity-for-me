# Makefile for TCameraPlotEvents
#
#
# 
#By: Oscar F Romero Matamala


BIN_DIR := .
EXE := $(BIN_DIR)/EventCleaning
DICT_DIR := ${EXACT_DIR}/dict

# Dictionary settings
# Pass both EventInfo.h and SimRunData.h so rootcling generates the ROOT I/O
# dictionary for both classes in a single pass (LinkDef.h pragma-links both).
DICT_SRC := EventInfoDict.cpp
DICT_OBJ := $(BIN_DIR)/EventInfoDict.o
DICT_HDR := EventInfo.h
LINKDEF  := LinkDef.h

# # root cling
# rootcling -f $(DICT_OBJ) -c $(DICT_HDR) $(LINKDEF)

ARCH := $(shell uname)

# linux flags
ifeq ($(ARCH),Linux)
CXX           = g++
CXXFLAGS      = -g -O3 -Wall -ggdb3 -fPIC -fno-strict-aliasing -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE_SOURCE -D_LARGEFILE64_SOURCE
LD            = g++
LDFLAGS       = -g
SOFLAGS       = -shared
endif

# Apple OS X flags
ifeq ($(ARCH),Darwin)
CXX           = g++ 
CXXFLAGS      = -g -O3 -Wall -fPIC  -fno-strict-aliasing
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

OutPutOpt     = -o

PREFIX		= /usr/local
# Root

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
ROOTLDFLAGS  := $(shell root-config --ldflags)
ROOTLIBDIR   := $(shell root-config --libdir)

CLLFLAGS += $(ROOTLDFLAGS)

CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTGLIBS)
LIBS         += -lMinuit -L./
LIBS		+= -L$(DICT_DIR)


INCLUDEFLAGS  = -Idict -Iinclude -I./inc/ -I$(DICT_DIR)/include -I./
CXXFLAGS     += $(INCLUDEFLAGS)

ALLFLAGS = $(CXXFLAGS) $(CPPFLAGS) -Wall

SRC := $(wildcard $(BIN_DIR)/*.cpp)
OBJ := $(SRC:$(BIN_DIR)/%.cpp=$(BIN_DIR)/%.o)



CP  := cp

#Recipe

.PHONY: all clean rootcling
all: $(EXE)

# Generate dictionary source with rootcling
$(DICT_SRC): $(DICT_HDR) $(LINKDEF)
	@echo "Rootcling..."
	/opt/root/bin/rootcling -f $@ -c $(DICT_HDR) $(LINKDEF)

# Compile dictionary object
$(DICT_OBJ): $(DICT_SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): $(OBJ) | $(BIN_DIR) 
	$(LD) $(OBJ) $(CXXFLAGS) $(LIBS) $(LDFLAGS) $(OutPutOpt) $@ -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -lExACT
	@echo "$@ Compilation done"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(LD) $(CXXFLAGS) -c $< -o $@



$(BIN_DIR)/$(OBJ_DIR):
		mkdir -p $@

clean: 

	@echo "Cleaning up..."
	rm -f $(BIN_DIR)/*.o $(EXE)
	rm -f $(BIN_DIR)/*.pcm $(EXE)
	rm -f $(BIN_DIR)/EventInfoDict* $(EXE)
	@echo "Clean complete."

rootcling:
	@echo "Running rootcling..."
	rm -f $(DICT_SRC) $(BIN_DIR)/EventInfoDict_rdict.pcm
	/opt/root/bin/rootcling -f $(DICT_SRC) -c $(DICT_HDR) $(LINKDEF)
	@echo "Rootcling complete"

-include $(OBJ:.o=.d)

.SUFFIXES: .o
