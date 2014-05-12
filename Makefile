SHELL=/bin/sh
.SUFFIXES:
.SUFFIXES: .cpp .h .o

CXX=g++
CXXFLAGS=-c -Wall -march=native -std=c++11 -DTERMINALCOLORS 
FASTFLAGS=-O3
DEBUGFLAGS=-g
CLUSTERCXXFLAGS=-c -Wall -std=c++11 -O3
#LDFLAGS=-pthread
LDFLAGS=
SRC_DIR=tool target
BUILD_DIR=build
SOURCES=$(foreach srcdir,$(SRC_DIR),$(wildcard $(srcdir)/*.cpp))
#OBJECTS=$(addprefix build/,${SOURCES:.cpp=.o})
OBJECTS=$(foreach srcdir,$(SRC_DIR),$(patsubst $(srcdir)/%.cpp,$(BUILD_DIR)/%.o,$(filter $(srcdir)/%.cpp,$(SOURCES))))
INCLUDES=$(addprefix -I,$(SRC_DIR))
vpath %.cpp $(SRC_DIR)
vpath %.h $(SRC_DIR)
vpath %.hpp $(SRC_DIR)
TITLE=lin

.PHONY : all clean

# make all
all: fast

# make fast
fast: CXXFLAGS += $(FASTFLAGS)
fast: $(TITLE)

# make fastdebug
fastdebug: CXXFLAGS += $(FASTFLAGS) $(DEBUGFLAGS)
fastdebug: $(TITLE)

# make debug
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(TITLE)

# make cluster
cluster: CXXFLAGS = $(CLUSTERCXXFLAGS)
cluster: $(TITLE)

# make
$(TITLE): $(OBJECTS) $(BUILD_DIR)/tinyxml2.o
	$(CXX) -g -o $@ $^ $(INCLUDES) $(LDFLAGS)

# make %.o
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -MMD -MF ./$@.d $(INCLUDES) $(LDFLAGS)

$(BUILD_DIR)/tinyxml2.o: tinyxml2/tinyxml2.cpp
	$(CXX) $(CXXFLAGS) -o $@ tinyxml2/tinyxml2.cpp -MMD -MF ./$@.d $(LDFLAGS)

# make clean
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d $(TITLE)

-include $(wildcard $(BUILD_DIR)/*.d)
