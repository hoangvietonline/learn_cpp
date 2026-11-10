# Build with clang++ on Mac. To use g++: make CXX=g++
CXX = clang++
CPPFLAGS = -Iinc
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0

APP = build/student_manager
DEBUG_APP = build/student_manager_debug
TEST_APP = build/test_main

SOURCES = src/main.cpp src/student.cpp src/fileio.cpp
OBJECTS = $(SOURCES:src/%.cpp=build/obj/%.o)
DEBUG_OBJECTS = $(SOURCES:src/%.cpp=build/debug/%.o)
TEST_OBJECTS = build/tests/test_main.o build/obj/student.o build/obj/fileio.o

.PHONY: all debug build-test test clean

# Default target: build the application.
all: $(APP)

debug: $(DEBUG_APP)

build-test: $(TEST_APP)

# Run tests in data/, as in the VS Code settings.
test: $(TEST_APP)
	cd data && ../$(TEST_APP)

# Link object files into programs.
$(APP): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(DEBUG_APP): $(DEBUG_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(TEST_APP): $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

# -c compiles each cpp to an object. -MMD -MP track included headers.
build/obj/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/debug/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -DDEBUG_LOG -MMD -MP -c $< -o $@

build/tests/%.o: tests/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Remove build files only. Keep source files and data.
clean:
	rm -rf build/obj build/debug build/tests
	rm -f $(APP) $(DEBUG_APP) $(TEST_APP)
	rm -rf $(APP).dSYM $(DEBUG_APP).dSYM $(TEST_APP).dSYM

-include $(OBJECTS:.o=.d) $(DEBUG_OBJECTS:.o=.d) build/tests/test_main.d
