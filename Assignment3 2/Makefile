# C++ Assignment 3
# Sources: src/, headers: include/, tests: tests/, output: build/.
# Windows (MinGW): mingw32-make
# Linux / macOS:   make

CXX = g++
CPPFLAGS += -Iinclude
CXXFLAGS += -std=c++17 -Wall -Wextra -g -O0 -pthread
LDLIBS += -pthread

ifeq ($(OS),Windows_NT)
EXE = .exe
else
EXE =
endif

APP = build/sensor_processor$(EXE)
TEST_APP = build/sensor_tests$(EXE)

COMMON_SOURCES = $(filter-out src/main.cpp,$(wildcard src/*.cpp))
COMMON_OBJECTS = $(patsubst src/%.cpp,build/%.o,$(COMMON_SOURCES))
APP_OBJECTS = build/main.o $(COMMON_OBJECTS)
TEST_OBJECTS = build/test_main.o $(COMMON_OBJECTS)
OBJECTS = $(sort $(APP_OBJECTS) $(TEST_OBJECTS))
DEPS = $(OBJECTS:.o=.d)

# Arguments are passed unchanged to main(argc, argv).
# Example (if your program accepts file then worker count):
# make run ARGS="data/sensor_log.txt 4"
ARGS ?=

.DEFAULT_GOAL := all
.PHONY: all build run test clean

all: $(APP)

build: all

$(APP): $(APP_OBJECTS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(TEST_APP): $(TEST_OBJECTS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

build/%.o: src/%.cpp | build-dir
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/%.o: tests/%.cpp | build-dir
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

.PHONY: build-dir
build-dir:
ifeq ($(OS),Windows_NT)
ifeq ($(findstring sh,$(SHELL)),)
	@if not exist build mkdir build
else
	@mkdir -p build
endif
else
	@mkdir -p build
endif

run: $(APP)
	./$(APP) $(ARGS)

test: $(TEST_APP)
	./$(TEST_APP)

# Native Windows cmd.exe needs del; MSYS / Unix shells use rm.
ifeq ($(OS),Windows_NT)
ifeq ($(findstring sh,$(SHELL)),)
clean:
	- del /Q $(subst /,\,$(APP) $(TEST_APP) $(OBJECTS) $(DEPS)) 2>NUL
else
clean:
	$(RM) $(APP) $(TEST_APP) $(OBJECTS) $(DEPS)
endif
else
clean:
	$(RM) $(APP) $(TEST_APP) $(OBJECTS) $(DEPS)
endif

-include $(DEPS)
