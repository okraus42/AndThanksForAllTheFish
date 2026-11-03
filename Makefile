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

# Detect pkg-config or its pkgconf alternative.
PKG_CONFIG ?= $(shell command -v pkg-config 2>/dev/null || command -v pkgconf 2>/dev/null)


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
SDL_PIC_OPTION = -DCMAKE_POSITION_INDEPENDENT_CODE=ON

# All SDL components are built statically.
SDL_STATIC_OPTIONS = \
	$(SDL_PIC_OPTION) \
	-DBUILD_SHARED_LIBS=OFF

SDL_CMAKE_OPTIONS = \
	$(SDL_PIC_OPTION) \
	-DSDL_SHARED=OFF \
	-DSDL_STATIC=ON \
	-DBUILD_SHARED_LIBS=OFF

SDL_IMAGE_CMAKE_OPTIONS = $(SDL_STATIC_OPTIONS)
SDL_TTF_CMAKE_OPTIONS = $(SDL_STATIC_OPTIONS)
SDL_MIXER_CMAKE_OPTIONS = $(SDL_STATIC_OPTIONS)

SDL_EXPECTED_LIBS = \
	$(BUILD_DIR)/lib/libSDL3.a \
	$(BUILD_DIR)/lib/libSDL3_image.a \
	$(BUILD_DIR)/lib/libSDL3_ttf.a \
	$(BUILD_DIR)/lib/libSDL3_mixer.a

SDL_PC_PACKAGES = sdl3-image sdl3-ttf sdl3-mixer


# ==========================================
# Platform Linker Configuration
# ==========================================

ifeq ($(UNAME_S),Linux)

	# Linux: static archives grouped to resolve circular dependencies.

	SDL_ARCHIVES = \
		-Wl,--start-group \
		$(BUILD_DIR)/lib/libSDL3_mixer.a \
		$(BUILD_DIR)/lib/libSDL3_ttf.a \
		$(BUILD_DIR)/lib/libSDL3_image.a \
		$(BUILD_DIR)/lib/libSDL3.a \
		-Wl,--end-group

	SDL_SYSTEM_LIBS = $(shell \
		PKG_CONFIG_PATH="$(SDL_INSTALL_DIR)/lib/pkgconfig$${PKG_CONFIG_PATH:+:$$PKG_CONFIG_PATH}" \
		"$(PKG_CONFIG)" --static --libs $(SDL_PC_PACKAGES) 2>/dev/null \
		| sed -E 's/(^| )-lSDL3(_image|_ttf|_mixer)?( |$$)/\1\3/g')

	SDL_LIB = $(SDL_ARCHIVES) $(SDL_SYSTEM_LIBS)


else ifeq ($(UNAME_S),Darwin)

	# macOS: static archives.
	# SDL's pkg-config metadata supplies the required Apple frameworks
	# and other transitive static dependencies.

	SDL_ARCHIVES = \
		$(BUILD_DIR)/lib/libSDL3_mixer.a \
		$(BUILD_DIR)/lib/libSDL3_ttf.a \
		$(BUILD_DIR)/lib/libSDL3_image.a \
		$(BUILD_DIR)/lib/libSDL3.a

	SDL_SYSTEM_LIBS = $(shell \
		PKG_CONFIG_PATH="$(SDL_INSTALL_DIR)/lib/pkgconfig$${PKG_CONFIG_PATH:+:$$PKG_CONFIG_PATH}" \
		"$(PKG_CONFIG)" --static --libs $(SDL_PC_PACKAGES) 2>/dev/null \
		| sed -E 's/(^| )-lSDL3(_image|_ttf|_mixer)?( |$$)/\1\3/g')

	SDL_LIB = $(SDL_ARCHIVES) $(SDL_SYSTEM_LIBS)


else

	$(error Unsupported operating system: $(UNAME_S))

endif


# ==========================================
# Main Build Rules
# ==========================================

all: $(NAME)

# Check the linker metadata tool before starting the SDL build.
check-pkg-config:
	@if [ -z "$(strip $(PKG_CONFIG))" ]; then \
		echo "ERROR: pkg-config/pkgconf was not found."; \
		if [ "$(UNAME_S)" = "Darwin" ]; then \
			echo "Install it on macOS with: brew install pkgconf"; \
		else \
			echo "Install pkg-config using your Linux package manager."; \
		fi; \
		exit 1; \
	fi

# SDL must be installed before compiling files that include SDL headers.
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

$(SDL_STAMP): | check-pkg-config
	mkdir -p "$(SDL_SRC_DIR)" "$(BUILD_DIR)"
	bash -eu -c '\
		clone_repo() { \
			name="$$1"; \
			url="$$2"; \
			if [ ! -d "$$name/.git" ]; then \
				if [ -e "$$name" ]; then \
					echo "ERROR: $$name exists but is not a Git repository."; \
					exit 1; \
				fi; \
				echo "Cloning $$name..."; \
				git clone "$$url" "$$name"; \
			fi; \
		}; \
		\
		clone_repo "$(SDL_DIR)" https://github.com/libsdl-org/SDL.git; \
		clone_repo "$(SDL_IMAGE_DIR)" https://github.com/libsdl-org/SDL_image.git; \
		clone_repo "$(SDL_TTF_DIR)" https://github.com/libsdl-org/SDL_ttf.git; \
		clone_repo "$(SDL_MIXER_DIR)" https://github.com/libsdl-org/SDL_mixer.git; \
		\
		BUILD_NEEDED=0; \
		\
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
		for cache in \
			"$(SDL_DIR)/build/CMakeCache.txt" \
			"$(SDL_IMAGE_DIR)/build/CMakeCache.txt" \
			"$(SDL_TTF_DIR)/build/CMakeCache.txt" \
			"$(SDL_MIXER_DIR)/build/CMakeCache.txt"; do \
			if ! grep -q "^CMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON$$" "$$cache" 2>/dev/null; then \
				BUILD_NEEDED=1; \
			fi; \
		done; \
		\
		if [ "$$BUILD_NEEDED" -eq 0 ]; then \
			echo "SDL static stack already built."; \
		else \
			echo "Building static SDL stack for $(UNAME_S)..."; \
			command -v cmake >/dev/null 2>&1 || { echo "ERROR: CMake is required."; exit 1; }; \
			command -v git >/dev/null 2>&1 || { echo "ERROR: Git is required."; exit 1; }; \
			rm -rf \
				"$(SDL_DIR)/build" \
				"$(SDL_IMAGE_DIR)/build" \
				"$(SDL_TTF_DIR)/build" \
				"$(SDL_MIXER_DIR)/build"; \
			\
			echo "Building SDL3..."; \
			cmake -S "$(SDL_DIR)" -B "$(SDL_DIR)/build" \
				-DCMAKE_INSTALL_PREFIX="$(SDL_INSTALL_DIR)" \
				-DCMAKE_BUILD_TYPE=Release \
				$(SDL_CMAKE_OPTIONS); \
			cmake --build "$(SDL_DIR)/build" --config Release --parallel; \
			cmake --install "$(SDL_DIR)/build" --config Release; \
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
			echo "Building SDL3_mixer..."; \
			cmake -S "$(SDL_MIXER_DIR)" -B "$(SDL_MIXER_DIR)/build" \
				-DCMAKE_INSTALL_PREFIX="$(SDL_INSTALL_DIR)" \
				-DCMAKE_PREFIX_PATH="$(SDL_INSTALL_DIR)" \
				-DSDL3_DIR="$(SDL_INSTALL_DIR)/lib/cmake/SDL3" \
				-DCMAKE_BUILD_TYPE=Release \
				$(SDL_MIXER_CMAKE_OPTIONS); \
			cmake --build "$(SDL_MIXER_DIR)/build" --config Release --parallel; \
			cmake --install "$(SDL_MIXER_DIR)/build" --config Release; \
		fi; \
		\
		for lib in $(SDL_EXPECTED_LIBS); do \
			if [ ! -f "$$lib" ]; then \
				echo "ERROR: Missing static library: $$lib"; \
				exit 1; \
			fi; \
		done; \
		\
		if [ ! -f "$(BUILD_DIR)/include/SDL3/SDL.h" ]; then \
			echo "ERROR: SDL headers were not installed."; \
			exit 1; \
		fi; \
		\
		export PKG_CONFIG_PATH="$(SDL_INSTALL_DIR)/lib/pkgconfig$${PKG_CONFIG_PATH:+:$$PKG_CONFIG_PATH}"; \
		"$(PKG_CONFIG)" --exists $(SDL_PC_PACKAGES) || { \
			echo "ERROR: SDL pkg-config metadata is missing or incomplete."; \
			exit 1; \
		}; \
		\
		echo "SDL static libraries ready."; \
		touch "$@"; \
		echo "SDL stack build completed."; \
		'


# ==========================================
# Cleaning
# ==========================================

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)
	rm -rf $(BUILD_DIR)

# Full reset: remove downloaded SDL sources and CMake caches.
deepclean: fclean
	rm -rf $(SDL_SRC_DIR)

re: deepclean all


# ==========================================
# Dependency Files
# ==========================================

-include $(DEP)

.PHONY: all clean fclean deepclean re sdl check-pkg-config
