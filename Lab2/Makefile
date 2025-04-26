CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O3 -Iinclude -fopenmp

LIB_DIR = lib
INCLUDE_DIR = include
BUILD_DIR = build

TARGET = Lab2
VERIFIER = verifier
OBJECTS := $(patsubst $(LIB_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(wildcard $(LIB_DIR)/*.cpp)) $(BUILD_DIR)/main.o
TESTCASES := $(wildcard testcase/*.hgr)

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
	for file in $(TESTCASES); do \
		echo ""; \
		./$(TARGET) $$file; \
		./$(VERIFIER) $$file output.txt; \
	done