# Run make from this directory. On Windows, use mingw32-make.
CXX = g++
CPPFLAGS += -Iinc
CXXFLAGS += -std=c++17 -Wall -Wextra -g -O0

# Keep normal and trace objects separate so changing modes rebuilds correctly.
MODE ?= normal
ifeq ($(MODE),trace)
CPPFLAGS += -DTRACE_LIFECYCLE
endif

BUILD_DIR := build/$(MODE)
BIN_DIR := bin/$(MODE)

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

COMMON_SRC := src/Device.cpp src/Light.cpp src/Thermostat.cpp \
              src/SecurityCam.cpp src/Room.cpp
COMMON_OBJ := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(COMMON_SRC))
MENU_OBJ := $(BUILD_DIR)/src/main.o
DEMO_OBJ := $(BUILD_DIR)/src/demo.o
TEST_OBJ := $(BUILD_DIR)/tests/test_main.o
ALL_OBJ := $(COMMON_OBJ) $(MENU_OBJ) $(DEMO_OBJ) $(TEST_OBJ)

MENU := $(BIN_DIR)/smart_home$(EXE)
DEMO := $(BIN_DIR)/demo$(EXE)
TEST := $(BIN_DIR)/test_main$(EXE)

.PHONY: all menu demo test run run-demo trace clean help

all: menu

menu: $(MENU)

demo: $(DEMO)

test: $(TEST)
	./$(TEST)

run: $(MENU)
	./$(MENU)

run-demo: $(DEMO)
	./$(DEMO)

# Example: make trace; make MODE=trace run
trace:
	$(MAKE) MODE=trace menu

$(MENU): $(COMMON_OBJ) $(MENU_OBJ)
ifeq ($(OS),Windows_NT)
	@if not exist "$(subst /,\,$(@D))" mkdir "$(subst /,\,$(@D))"
else
	@mkdir -p "$(@D)"
endif
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(DEMO): $(COMMON_OBJ) $(DEMO_OBJ)
ifeq ($(OS),Windows_NT)
	@if not exist "$(subst /,\,$(@D))" mkdir "$(subst /,\,$(@D))"
else
	@mkdir -p "$(@D)"
endif
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(TEST): $(COMMON_OBJ) $(TEST_OBJ)
ifeq ($(OS),Windows_NT)
	@if not exist "$(subst /,\,$(@D))" mkdir "$(subst /,\,$(@D))"
else
	@mkdir -p "$(@D)"
endif
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

# -MMD -MP track included headers and rebuild objects when headers change.
$(BUILD_DIR)/%.o: %.cpp
ifeq ($(OS),Windows_NT)
	@if not exist "$(subst /,\,$(@D))" mkdir "$(subst /,\,$(@D))"
else
	@mkdir -p "$(@D)"
endif
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	@if exist build rmdir /s /q build
	@if exist bin rmdir /s /q bin
else
	rm -rf build bin
endif

help:
	@echo "make                  Build the menu program"
	@echo "make run              Build and run the menu"
	@echo "make demo             Build src/demo.cpp"
	@echo "make run-demo         Build and run the demo"
	@echo "make test             Build and run tests/test_main.cpp"
	@echo "make trace            Build the menu with lifecycle logs"
	@echo "make MODE=trace demo  Build the demo with lifecycle logs"
	@echo "make MODE=trace test  Run tests with lifecycle logs"
	@echo "make clean            Remove build and bin directories"

-include $(ALL_OBJ:.o=.d)
