# FixItNow - Service Management
#
#   make           build the program (incremental: only changed files are recompiled)
#   make run       build, then run it
#   make clean     delete the program and the build/ folder
#   make rebuild   clean + build
#
# On Windows with MinGW, use "mingw32-make" instead of "make".

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I./include
DEPFLAGS := -MMD -MP

SRC := main.cpp $(wildcard src/*.cpp)
OBJ := $(patsubst %.cpp,build/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

ifeq ($(OS),Windows_NT)
  # Always use cmd.exe so the commands below work the same from any terminal
  SHELL  := cmd.exe
  EXE    := main.exe
  RUN    := $(EXE)
  MKDIR   = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
  CLEAN  := if exist build rmdir /S /Q build & if exist $(EXE) del /Q $(EXE)
else
  EXE    := main
  RUN    := ./$(EXE)
  MKDIR   = mkdir -p $(1)
  CLEAN  := rm -rf build $(EXE)
endif

.PHONY: all run clean rebuild

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

build/%.o: %.cpp
	@$(call MKDIR,$(@D))
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

run: $(EXE)
	$(RUN)

clean:
	$(CLEAN)

rebuild: clean
	@$(MAKE) --no-print-directory all

# Recompile a .cpp when any header it includes changes
-include $(DEP)
