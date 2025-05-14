CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O3 -Iinc -fopenmp

LIB_DIR = src
INCLUDE_DIR = inc
BUILD_DIR = build

TARGET = D2DGRter
OBJECTS := $(patsubst $(LIB_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(wildcard $(LIB_DIR)/*.cpp)) $(BUILD_DIR)/main.o

num ?= 0
case ?=publicCase
TESTPATH = $(case)/testcase$(num)
TESTNAME = testcase$(num)

all: clean $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR)/main.o: main.cpp $(INCLUDE_DIR)/*.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run:
	timeout 1200s ./$(TARGET) $(TESTPATH)/$(TESTNAME).gmp $(TESTPATH)/$(TESTNAME).gcl $(TESTPATH)/$(TESTNAME).cst $(TESTPATH)/$(TESTNAME).lg
	./Evaluator $(TESTPATH) $(TESTNAME)