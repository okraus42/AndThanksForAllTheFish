
# ==========================================
# Project
# ==========================================

NAME = atfatf_game

CXX = g++-15
CXXFLAGS = -Wall -Wextra -Werror -MMD -MP -std=c++20
LDFLAGS =

# ==========================================
# Platform
# ==========================================

UNAME_S := $(shell uname -s)

# ==========================================
# Directories
# ==========================================

ROOT_DIR = $(abspath .)

SRC_DIR = src
INC_DIR = include

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

SDL_SRC_DIR = .sdl_src
SDL_DIR = $(SDL_SRC_DIR)/SDL
SDL_IMAGE_DIR = $(SDL_SRC_DIR)/SDL_image
SDL_TTF_DIR = $(SDL_SRC_DIR)/SDL_ttf
SDL_MIXER_DIR = $(SDL_SRC_DIR)/SDL_mixer

SDL_INSTALL_DIR = $(ROOT_DIR)/$(BUILD_DIR)

SDL_STAMP = $(BUILD_DIR)/.sdl-built

# ==========================================
# Sources
# ==========================================

SRC = \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/audio/SDLAudio.cpp \
	$(SRC_DIR)/input/TerminalInput.cpp \
	$(SRC_DIR)/input/SDLInput.cpp \
	$(SRC_DIR)/debug/MazeHistory.cpp \
	$(SRC_DIR)/engine/Engine.cpp \
	$(SRC_DIR)/maze/Maze.cpp \
	$(SRC_DIR)/maze/MazeEditor.cpp \
	$(SRC_DIR)/maze/MazeGenerator.cpp \
	$(SRC_DIR)/map/Map.cpp \
	$(SRC_DIR)/map/BoolOverlay.cpp \
	$(SRC_DIR)/map/MapEditor.cpp \
	$(SRC_DIR)/render/SDLRenderer.cpp \
	$(SRC_DIR)/render/TerminalRenderer.cpp \
	$(SRC_DIR)/games/atfatf/AtfatfGame.cpp \
	$(SRC_DIR)/utils/getTime.cpp \
	$(SRC_DIR)/utils/logger.cpp \
	$(SRC_DIR)/games/atfatf/states/AtfatfMenuState.cpp \
	$(SRC_DIR)/games/atfatf/states/AtfatfGameState.cpp \
	$(SRC_DIR)/games/atfatf/states/AtfatfHighscoreState.cpp \
	$(SRC_DIR)/games/atfatf/states/AtfatfNameState.cpp

OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEP = $(OBJ:.o=.d)

# ==========================================
# Includes
# ==========================================

INCLUDES = \
	-I$(INC_DIR) \
	-I$(SRC_DIR) \
	-I$(BUILD_DIR)/include

# ==========================================
# SDL Configuration
# ==========================================

# Build SDL object files as position-independent code.
# This allows static SDL archives to be linked into a PIE executable.

SDL_PIC_OPTION = -DCMAKE_POSITION_INDEPENDENT_CODE=ON

ifeq ($(UNAME_S),Linux)

	# --------------------------------------
	# Linux: Static SDL
	# --------------------------------------

	SDL_CMAKE_OPTIONS = \
		$(SDL_PIC_OPTION) \
		-DSDL_SHARED=OFF \
		-DSDL_STATIC=ON

	SDL_IMAGE_CMAKE_OPTIONS = \
		$(SDL_PIC_OPTION) \
		-DBUILD_SHARED_LIBS=OFF

	SDL_TTF_CMAKE_OPTIONS = \
		$(SDL_PIC_OPTION) \
		-DBUILD_SHARED_LIBS=OFF

	SDL_MIXER_CMAKE_OPTIONS = \
		$(SDL_PIC_OPTION) \
		-DBUILD_SHARED_LIBS=OFF

	SDL_EXPECTED_LIBS = \
		$(BUILD_DIR)/lib/libSDL3.a \
		$(BUILD_DIR)/lib/libSDL3_image.a \
		$(BUILD_DIR)/lib/libSDL3_ttf.a \
		$(BUILD_DIR)/lib/libSDL3_mixer.a

	# Static SDL archives.
	# The linker group handles circular dependencies between archives.

	SDL_ARCHIVES = \
		-Wl,--start-group \
		$(BUILD_DIR)/lib/libSDL3_mixer.a \
		$(BUILD_DIR)/lib/libSDL3_ttf.a \
		$(BUILD_DIR)/lib/libSDL3_image.a \
		$(BUILD_DIR)/lib/libSDL3.a \
		-Wl,--end-group

	# Obtain transitive system dependencies from pkg-config.
	# SDL libraries themselves are removed because we link their
	# static archives explicitly above.

	SDL_SYSTEM_LIBS = $(shell \
		PKG_CONFIG_PATH="$(ROOT_DIR)/$(BUILD_DIR)/lib/pkgconfig$${PKG_CONFIG_PATH:+:$$PKG_CONFIG_PATH}" \
		pkg-config --static --libs sdl3-image sdl3-ttf sdl3-mixer 2>/dev/null \
		| sed -E 's/(^| )-lSDL3(_image|_ttf|_mixer)?( |$$)/\1\3/g')

	SDL_LIB = $(SDL_ARCHIVES) $(SDL_SYSTEM_LIBS)

else ifeq ($(UNAME_S),Darwin)

	# --------------------------------------
	# macOS: Shared SDL
	# --------------------------------------

	SDL_CMAKE_OPTIONS = \
		$(SDL_PIC_OPTION)

	SDL_IMAGE_CMAKE_OPTIONS = \
		$(SDL_PIC_OPTION)

	SDL_TTF_CMAKE_OPTIONS = \
		$(SDL_PIC_OPTION)

	SDL_MIXER_CMAKE_OPTIONS = \
		$(SDL_PIC_OPTION)

	SDL_EXPECTED_LIBS = \
		$(BUILD_DIR)/lib/libSDL3.dylib \
		$(BUILD_DIR)/lib/libSDL3_image.dylib \
		$(BUILD_DIR)/lib/libSDL3_ttf.dylib \
		$(BUILD_DIR)/lib/libSDL3_mixer.dylib

	SDL_LIB = \
		-L$(BUILD_DIR)/lib \
		-lSDL3 \
		-lSDL3_image \
		-lSDL3_ttf \
		-lSDL3_mixer \
		-Wl,-rpath,@executable_path/$(BUILD_DIR)/lib

else

	$(error Unsupported operating system: $(UNAME_S))

endif

# ==========================================
# Main Build Rules
# ==========================================

all: $(NAME)

# SDL must be installed before compiling files that include its headers.

$(OBJ): | $(SDL_STAMP)

$(NAME): $(OBJ) | $(SDL_STAMP)
	$(CXX) $(LDFLAGS) $(OBJ) $(SDL_LIB) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ==========================================
# SDL Build
# ==========================================

sdl: $(SDL_STAMP)

$(SDL_STAMP):
	set -eu; \
	mkdir -p "$(SDL_SRC_DIR)"; \
	\
	# -------------------------------------- \
	# Clone SDL repositories if necessary. \
	# -------------------------------------- \
	\
	if [ ! -d "$(SDL_DIR)/.git" ]; then \
		if [ -e "$(SDL_DIR)" ]; then \
			echo "ERROR: $(SDL_DIR) exists but is not a Git repository."; \
			exit 1; \
		fi; \
		echo "Cloning SDL..."; \
		git clone https://github.com/libsdl-org/SDL.git "$(SDL_DIR)"; \
	fi; \
	\
	if [ ! -d "$(SDL_IMAGE_DIR)/.git" ]; then \
		if [ -e "$(SDL_IMAGE_DIR)" ]; then \
			echo "ERROR: $(SDL_IMAGE_DIR) exists but is not a Git repository."; \
			exit 1; \
		fi; \
		echo "Cloning SDL_image..."; \
		git clone https://github.com/libsdl-org/SDL_image.git "$(SDL_IMAGE_DIR)"; \
	fi; \
	\
	if [ ! -d "$(SDL_TTF_DIR)/.git" ]; then \
		if [ -e "$(SDL_TTF_DIR)" ]; then \
			echo "ERROR: $(SDL_TTF_DIR) exists but is not a Git repository."; \
			exit 1; \
		fi; \
		echo "Cloning SDL_ttf..."; \
		git clone https://github.com/libsdl-org/SDL_ttf.git "$(SDL_TTF_DIR)"; \
	fi; \
	\
	if [ ! -d "$(SDL_MIXER_DIR)/.git" ]; then \
		if [ -e "$(SDL_MIXER_DIR)" ]; then \
			echo "ERROR: $(SDL_MIXER_DIR) exists but is not a Git repository."; \
			exit 1; \
		fi; \
		echo "Cloning SDL_mixer..."; \
		git clone https://github.com/libsdl-org/SDL_mixer.git "$(SDL_MIXER_DIR)"; \
	fi; \
	\
	# -------------------------------------- \
	# Check whether SDL needs rebuilding. \
	# -------------------------------------- \
	\
	BUILD_NEEDED=0; \
	for lib in $(SDL_EXPECTED_LIBS); do \
		if [ ! -f "$$lib" ]; then \
			BUILD_NEEDED=1; \
		fi; \
	done; \
	\
	if [ ! -f "$(BUILD_DIR)/include/SDL3/SDL.h" ]; then \
		BUILD_NEEDED=1; \
	fi; \
	\
	if [ "$(UNAME_S)" = "Linux" ]; then \
		for cache in \
			"$(SDL_DIR)/build/CMakeCache.txt" \
			"$(SDL_IMAGE_DIR)/build/CMakeCache.txt" \
			"$(SDL_TTF_DIR)/build/CMakeCache.txt" \
			"$(SDL_MIXER_DIR)/build/CMakeCache.txt"; do \
			if ! grep -q '^CMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON$$' "$$cache" 2>/dev/null; then \
				BUILD_NEEDED=1; \
			fi; \
		done; \
	fi; \
	\
	if [ "$$BUILD_NEEDED" -eq 0 ]; then \
		echo "SDL stack already built."; \
	else \
		\
		# -------------------------------------- \
		# Build prerequisites. \
		# -------------------------------------- \
		\
		echo "Building SDL stack for $(UNAME_S)..."; \
		command -v cmake >/dev/null 2>&1 || { echo "ERROR: CMake is required."; exit 1; }; \
		command -v git >/dev/null 2>&1 || { echo "ERROR: Git is required."; exit 1; }; \
		\
		if [ "$(UNAME_S)" = "Linux" ]; then \
			command -v pkg-config >/dev/null 2>&1 || { echo "ERROR: pkg-config is required."; exit 1; }; \
		fi; \
		\
		# Remove old CMake caches and object files. \
		rm -rf \
			"$(SDL_DIR)/build" \
			"$(SDL_IMAGE_DIR)/build" \
			"$(SDL_TTF_DIR)/build" \
			"$(SDL_MIXER_DIR)/build"; \
		\
		# -------------------------------------- \
		# Build SDL3. \
		# -------------------------------------- \
		\
		echo "Building SDL3..."; \
		cmake -S "$(SDL_DIR)" -B "$(SDL_DIR)/build" \
			-DCMAKE_INSTALL_PREFIX="$(SDL_INSTALL_DIR)" \
			-DCMAKE_BUILD_TYPE=Release \
			$(SDL_CMAKE_OPTIONS); \
		cmake --build "$(SDL_DIR)/build" --config Release --parallel; \
		cmake --install "$(SDL_DIR)/build" --config Release; \
		\
		# -------------------------------------- \
		# Build SDL3_image. \
		# -------------------------------------- \
		\
		echo "Building SDL3_image..."; \
		cmake -S "$(SDL_IMAGE_DIR)" -B "$(SDL_IMAGE_DIR)/build" \
			-DCMAKE_INSTALL_PREFIX="$(SDL_INSTALL_DIR)" \
			-DCMAKE_PREFIX_PATH="$(SDL_INSTALL_DIR)" \
			-DSDL3_DIR="$(SDL_INSTALL_DIR)/lib/cmake/SDL3" \
			-DCMAKE_BUILD_TYPE=Release \
			$(SDL_IMAGE_CMAKE_OPTIONS); \
		cmake --build "$(SDL_IMAGE_DIR)/build" --config Release --parallel; \
		cmake --install "$(SDL_IMAGE_DIR)/build" --config Release; \
		\
		# -------------------------------------- \
		# Build SDL3_ttf. \
		# -------------------------------------- \
		\
		echo "Building SDL3_ttf..."; \
		cmake -S "$(SDL_TTF_DIR)" -B "$(SDL_TTF_DIR)/build" \
			-DCMAKE_INSTALL_PREFIX="$(SDL_INSTALL_DIR)" \
			-DCMAKE_PREFIX_PATH="$(SDL_INSTALL_DIR)" \
			-DSDL3_DIR="$(SDL_INSTALL_DIR)/lib/cmake/SDL3" \
			-DCMAKE_BUILD_TYPE=Release \
			$(SDL_TTF_CMAKE_OPTIONS); \
		cmake --build "$(SDL_TTF_DIR)/build" --config Release --parallel; \
		cmake --install "$(SDL_TTF_DIR)/build" --config Release; \
		\
		# -------------------------------------- \
		# Build SDL3_mixer. \
		# -------------------------------------- \
		\
		echo "Building SDL3_mixer..."; \
		cmake -S "$(SDL_MIXER_DIR)" -B "$(SDL_MIXER_DIR)/build" \
			-DCMAKE_INSTALL_PREFIX="$(SDL_INSTALL_DIR)" \
			-DCMAKE_PREFIX_PATH="$(SDL_INSTALL_DIR)" \
			-DSDL3_DIR="$(SDL_INSTALL_DIR)/lib/cmake/SDL3" \
			-DCMAKE_BUILD_TYPE=Release \
			$(SDL_MIXER_CMAKE_OPTIONS); \
		cmake --build "$(SDL_MIXER_DIR)/build" --config Release --parallel; \
		cmake --install "$(SDL_MIXER_DIR)/build" --config Release; \
		\
		echo "SDL stack build completed."; \
	fi; \
	\
	# -------------------------------------- \
	# Verify installed SDL files. \
	# -------------------------------------- \
	\
	if [ "$(UNAME_S)" = "Linux" ]; then \
		export PKG_CONFIG_PATH="$(ROOT_DIR)/$(BUILD_DIR)/lib/pkgconfig$${PKG_CONFIG_PATH:+:$$PKG_CONFIG_PATH}"; \
		pkg-config --exists sdl3-image sdl3-ttf sdl3-mixer || { \
			echo "ERROR: SDL pkg-config metadata is missing."; \
			echo "Check $(BUILD_DIR)/lib/pkgconfig."; \
			exit 1; \
		}; \
		echo "SDL static libraries ready."; \
	else \
		echo "SDL shared libraries ready."; \
	fi; \
	\
	touch "$@"; \
	echo "SDL stack build completed."

# ==========================================
# Cleaning
# ==========================================

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)
	rm -rf $(BUILD_DIR)

# Full reset: remove downloaded SDL sources and all CMake caches.

deepclean: fclean
	rm -rf $(SDL_SRC_DIR)

re: deepclean all

# ==========================================
# Dependency Files
# ==========================================

-include $(DEP)

.PHONY: all clean fclean deepclean re sdl
