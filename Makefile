# ==========================================
# Project
# ==========================================

NAME = atfatf

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -MMD -MP -std=c++20

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

# ==========================================
# Sources
# ==========================================


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

INCLUDES = -I$(INC_DIR) -I$(SRC_DIR) -I$(BUILD_DIR)/include

# ==========================================
# SDL Libraries
# ==========================================

SDL_LIB = -L$(BUILD_DIR)/lib \
	-lSDL3 \
	-lSDL3_image \
	-lSDL3_ttf \
	-lSDL3_mixer

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	SDL_LIB += -Wl,-rpath,@executable_path/$(BUILD_DIR)/lib
endif

# ==========================================
# Build Rules
# ==========================================

all: sdl $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) $(SDL_LIB) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@


# ==========================================
# SDL Build
# ==========================================

sdl:
	@set -eu; \
	mkdir -p "$(SDL_SRC_DIR)"; \
	if [ ! -d "$(SDL_DIR)/.git" ]; then \
		if [ -e "$(SDL_DIR)" ]; then \
			echo "ERROR: $(SDL_DIR) exists but is not a Git repository."; \
			exit 1; \
		fi; \
		echo "Cloning SDL..."; \
		git clone https://github.com/libsdl-org/SDL.git "$(SDL_DIR)"; \
	fi; \
	if [ ! -d "$(SDL_IMAGE_DIR)/.git" ]; then \
		if [ -e "$(SDL_IMAGE_DIR)" ]; then \
			echo "ERROR: $(SDL_IMAGE_DIR) exists but is not a Git repository."; \
			exit 1; \
		fi; \
		echo "Cloning SDL_image..."; \
		git clone https://github.com/libsdl-org/SDL_image.git "$(SDL_IMAGE_DIR)"; \
	fi; \
	if [ ! -d "$(SDL_TTF_DIR)/.git" ]; then \
		if [ -e "$(SDL_TTF_DIR)" ]; then \
			echo "ERROR: $(SDL_TTF_DIR) exists but is not a Git repository."; \
			exit 1; \
		fi; \
		echo "Cloning SDL_ttf..."; \
		git clone https://github.com/libsdl-org/SDL_ttf.git "$(SDL_TTF_DIR)"; \
	fi; \
	if [ ! -d "$(SDL_MIXER_DIR)/.git" ]; then \
		if [ -e "$(SDL_MIXER_DIR)" ]; then \
			echo "ERROR: $(SDL_MIXER_DIR) exists but is not a Git repository."; \
			exit 1; \
		fi; \
		echo "Cloning SDL_mixer..."; \
		git clone https://github.com/libsdl-org/SDL_mixer.git "$(SDL_MIXER_DIR)"; \
	fi; \
	if [ -f "$(BUILD_DIR)/include/SDL3/SDL.h" ] && \
		[ -f "$(BUILD_DIR)/lib/libSDL3.dylib" ] && \
		[ -f "$(BUILD_DIR)/lib/libSDL3_image.dylib" ] && \
		[ -f "$(BUILD_DIR)/lib/libSDL3_ttf.dylib" ] && \
		[ -f "$(BUILD_DIR)/lib/libSDL3_mixer.dylib" ]; then \
		echo "SDL stack already installed."; \
	else \
		echo "Building SDL stack..."; \
		command -v cmake >/dev/null 2>&1 || { echo "ERROR: CMake is required."; exit 1; }; \
		cmake -S "$(SDL_DIR)" -B "$(SDL_DIR)/build" \
			-DCMAKE_INSTALL_PREFIX="$(SDL_INSTALL_DIR)" \
			-DCMAKE_BUILD_TYPE=Release; \
		cmake --build "$(SDL_DIR)/build" --config Release --parallel; \
		cmake --install "$(SDL_DIR)/build" --config Release; \
		cmake -S "$(SDL_IMAGE_DIR)" -B "$(SDL_IMAGE_DIR)/build" \
			-DCMAKE_INSTALL_PREFIX="$(SDL_INSTALL_DIR)" \
			-DCMAKE_PREFIX_PATH="$(SDL_INSTALL_DIR)" \
			-DCMAKE_BUILD_TYPE=Release; \
		cmake --build "$(SDL_IMAGE_DIR)/build" --config Release --parallel; \
		cmake --install "$(SDL_IMAGE_DIR)/build" --config Release; \
		cmake -S "$(SDL_TTF_DIR)" -B "$(SDL_TTF_DIR)/build" \
			-DCMAKE_INSTALL_PREFIX="$(SDL_INSTALL_DIR)" \
			-DCMAKE_PREFIX_PATH="$(SDL_INSTALL_DIR)" \
			-DCMAKE_BUILD_TYPE=Release; \
		cmake --build "$(SDL_TTF_DIR)/build" --config Release --parallel; \
		cmake --install "$(SDL_TTF_DIR)/build" --config Release; \
		cmake -S "$(SDL_MIXER_DIR)" -B "$(SDL_MIXER_DIR)/build" \
			-DCMAKE_INSTALL_PREFIX="$(SDL_INSTALL_DIR)" \
			-DCMAKE_PREFIX_PATH="$(SDL_INSTALL_DIR)" \
			-DCMAKE_BUILD_TYPE=Release; \
		cmake --build "$(SDL_MIXER_DIR)/build" --config Release --parallel; \
		cmake --install "$(SDL_MIXER_DIR)/build" --config Release; \
		echo "SDL stack build completed."; \
	fi

# ==========================================
# Cleaning
# ==========================================

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)
	rm -rf $(BUILD_DIR)

deepclean: fclean
	rm -rf $(SDL_SRC_DIR)

re: deepclean all

# ==========================================
# Dependency Files
# ==========================================

-include $(DEP)

.PHONY: all clean fclean deepclean re sdl