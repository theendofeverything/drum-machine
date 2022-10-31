CXXFLAGS_BASE := -std=c++20 -Wall -Wextra -Wpedantic
# ---< FIND MY HEADERS >---
CXXFLAGS_INC := -Iinc
# pkgconf: $ echo $PKG_CONFIG_PATH
# ---< SDL >---
CXXFLAGS_SDL := `pkgconf --cflags sdl2`
LDLIBS_SDL := `pkgconf --libs sdl2`
# ---< SDL Audio >---
CXXFLAGS_SDL_MIXER := `pkgconf --cflags SDL2_mixer`
LDLIBS_SDL_MIXER := `pkgconf --libs SDL2_mixer`
# ---< Put it all together >---
CXXFLAGS := $(CXXFLAGS_BASE) $(CXXFLAGS_INC) $(CXXFLAGS_SDL) $(CXXFLAGS_SDL_MIXER)
LDLIBS := $(LDLIBS_SDL) $(LDLIBS_SDL_MIXER)

# ---< Define my make variables >---
# Identify the main source file
SRC := src/main.cpp
# Put the build directory in .gitignore
BUILD_DIR := build
# HEADER_LIST : list of dependenices output by the compiler
HEADER_LIST := $(BUILD_DIR)/$(basename $(notdir $(SRC))).d
# Final executable
EXE := $(BUILD_DIR)/$(basename $(notdir $(SRC)))

default-target: $(EXE)

# ---< ImGUI >---
# IMGUI_DIR := ../../imgui
# SOURCES := $(SRC)

# ---< Check my make variables >---
# run with 'make what'
.PHONY: what
what:
	@echo --- My make variables ---
	@echo
	@echo expect SRC : src/main.cpp
	@echo SRC : $(SRC)
	@echo
	@echo expect HEADER_LIST : build/main.d
	@echo HEADER_LIST : $(HEADER_LIST)
	@echo
	@echo expect EXE : build/main
	@echo EXE : $(EXE)

.PHONY: how
how:
	@echo --- Build and Run ---
	@echo
	@echo "             Vim shortcut     Vim command"
	@echo "             ------------     -----------"
	@echo "Build        ;<Space>         :make -B"
	@echo "Run          ;r<Space>        :!./build/main.exe"
	@echo "Run at xyhw  ;w<Space>        :call RunInVimWindow('./build/main.exe')"
	@echo "Echo xyhw    ;W<Space>        :echo GetWindowXYWHinPixels()"
	@echo "Make tags    ;t<Space>        :make tags"

# =====[ EXE ]=====

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)
	@echo Run: ./$(EXE)
	@echo Specify window x y w h:
	@echo Run: ./$(EXE) 680 15 680 316


# =====[ TAGS ]=====

# --- Format the dependencies list for ctags ---
# Build a simple C program that generates build/headers.txt.
# Run the program like this (see 'tags' recipe below):
# ./ctags-dlist build/main.d
.PHONY: ctags-dlist
$(BUILD_DIR)/ctags-dlist: ctags-dlist.cpp
	$(CXX) $(CXXFLAGS_BASE) -o $@ $^

# --- Get the list of dependencies ---
# The expanded recipe for listing dependencies looks like this:
# g++ -std=c++20 -Wall -Wextra -Wpedantic -M main.cpp -MF main.d
.PHONY: $(HEADER_LIST)
$(HEADER_LIST): $(SRC)
	$(CXX) $(CXXFLAGS) -M $^ -MF $@

# --- Make the tags file ---
# Usually ctags only tags the source code I write.
# Use the dependencies list to include lib tags!
# `./ctags-dlist main.d` <---- creates file 'headers.txt'.
# Generate lib tags from 'headers.txt'.
# Then append my source code tags to that tags file.
.PHONY: tags
tags: $(HEADER_LIST) $(BUILD_DIR)/ctags-dlist
	$(BUILD_DIR)/ctags-dlist $(HEADER_LIST)
	ctags --c-kinds=+p+x -L headers.txt
	ctags --c-kinds=+p+x+l -a $(SRC)

