SHELL=/bin/sh
.SUFFIXES:
.SUFFIXES: .cpp .h .o

CXX=g++
#CXXFLAGS=-c -Wall -g -std=c++11
CXXFLAGS=-c -Wall -O3 -std=c++11
#LDFLAGS=-pthread
LDFLAGS=
SRC_DIR=tool
BUILD_DIR=build
SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
#OBJECTS=$(addprefix build/,${SOURCES:.cpp=.o})
OBJECTS=$(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
INCLUDES=-I$(SRC_DIR)
vpath %.cpp $(SRC_DIR)
#VPATH=$(SRC_DIR)
TITLE=lin

.PHONY : all clean

# make all
all: $(TITLE)

# make
$(TITLE): $(OBJECTS)
	$(CXX) -g -o $@ $^ $(LDFLAGS)

# make %.o
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -MMD -MF ./$@.d $(LDFLAGS)

# make clean
clean :
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d $(TITLE)

-include $(wildcard ./*.d)
