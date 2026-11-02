#include "SDLRenderer.hpp"
#include "logger.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <format>

SDLRenderer::SDLRenderer()
    : window(nullptr), renderer(nullptr), tileSize(60) {}

SDLRenderer::~SDLRenderer() {
    shutdown();
}

bool SDLRenderer::loadTexture(const std::string& path, SDL_Texture*& outTexture) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "IMG_Load failed (" << path << "): " << SDL_GetError() << std::endl;
        return false;
    }

    outTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!outTexture) {
        std::cerr << "SDL_CreateTextureFromSurface failed (" << path << "): " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

bool SDLRenderer::init() {

	
	windowWidth = 1920;
	windowHeight = 1080;
	textHeight = 60.0f;
	frame = 0;
	if (!TTF_Init()) {
		std::cerr << "TTF_Init failed: " << SDL_GetError() << std::endl;
		return false;
	}

	fontSweet16mono = TTF_OpenFont("./assets/fonts/Sweet16/Sweet16mono.ttf", textHeight);
	if (!fontSweet16mono) {
		std::cerr << "TTF_OpenFont failed: " << SDL_GetError() << std::endl;
		return false;
	}
	ttfInitialized = true;

    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Atfatf", windowWidth, windowHeight, SDL_WINDOW_FULLSCREEN);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        return false;
    }

	if (!SDL_HideCursor()) {
		std::cerr << "Could not hide cursor" << std::endl;
	}

    // Load textures here (fishTexture, etc.)
    if (!loadTexture("assets/sprites/fish1.png", fishTexture[0])) {
        return false;
    }
    if (!loadTexture("assets/sprites/fish2.png", fishTexture[1])) {
        return false;
    }
    // 0: right, 1: down, 2: left, 3: up
    if (!loadTexture("assets/sprites/dolphin_right.png", dolphinTextures[0][0])) {
        return false;
    }
	dolphinTextures[0][1] = dolphinTextures[0][0];
    dolphinTextures[0][2] = dolphinTextures[0][0];
    dolphinTextures[0][3] = dolphinTextures[0][0];
    if (!loadTexture("assets/sprites/dolphin_down.png", dolphinTextures[1][0])) {
        return false;
    }
    dolphinTextures[1][1] = dolphinTextures[1][0];
    dolphinTextures[1][2] = dolphinTextures[1][0];
    dolphinTextures[1][3] = dolphinTextures[1][0];
    if (!loadTexture("assets/sprites/dolphin_left.png", dolphinTextures[2][0])) {
        return false;
    }
    dolphinTextures[2][1] = dolphinTextures[2][0];
    dolphinTextures[2][2] = dolphinTextures[2][0];
    dolphinTextures[2][3] = dolphinTextures[2][0];
    if (!loadTexture("assets/sprites/dolphin_up.png", dolphinTextures[3][0])) {
        return false;
    }
    dolphinTextures[3][1] = dolphinTextures[3][0];
    dolphinTextures[3][2] = dolphinTextures[3][0];
    dolphinTextures[3][3] = dolphinTextures[3][0];
    if (!loadTexture("assets/sprites/shark_right.png", sharkTextures[0][0])) {
        return false;
    }
	sharkTextures[0][1] = sharkTextures[0][0];
    sharkTextures[0][2] = sharkTextures[0][0];
    sharkTextures[0][3] = sharkTextures[0][0];
    if (!loadTexture("assets/sprites/shark_down.png", sharkTextures[1][0])) {
        return false;
    }
    sharkTextures[1][1] = sharkTextures[1][0];
    sharkTextures[1][2] = sharkTextures[1][0];
    sharkTextures[1][3] = sharkTextures[1][0];
    if (!loadTexture("assets/sprites/shark_left.png", sharkTextures[2][0])) {
        return false;
    }
    sharkTextures[2][1] = sharkTextures[2][0];
    sharkTextures[2][2] = sharkTextures[2][0];
    sharkTextures[2][3] = sharkTextures[2][0];
    if (!loadTexture("assets/sprites/shark_up.png", sharkTextures[3][0])) {
        return false;
    }
    sharkTextures[3][1] = sharkTextures[3][0];
    sharkTextures[3][2] = sharkTextures[3][0];
    sharkTextures[3][3] = sharkTextures[3][0];
    if (!loadTexture("assets/sprites/shark_target.png", sharkTargetTexture)) { 
        return false;
    }
    if (!loadTexture("assets/sprites/exit_open.png", exitOpenTexture)) { 
        return false;
    }
	if (!loadTexture("assets/sprites/exit_closed.png", exitClosedTexture)) { 
        return false;
    }
	if (!loadTexture("assets/sprites/wall.png", wallTexture)) { 
        return false;
    }
	if (!loadTexture("assets/sprites/seaweed.png", seaweedTexture)) { 
        return false;
    }
    if (!loadTexture("assets/sprites/water.png", waterTexture)) {
        return false;
    }
	LOG_TRACE("here");
	for (int i = 1; i <= 32; i++) {
		std::string path = std::format("assets/sprites/heart{:02}.png", i);
		if (!loadTexture(path, heartTextures[i % 32])) {
			LOG_ERROR("Failed to load texture: {}", path);
			return false;
		}
    }
	LOG_TRACE("there");
    return true;
}

void SDLRenderer::clear() {
    // Set background to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void SDLRenderer::present() {
	++frame;
	LOG_TRACE("presenting frame {}", frame);
    SDL_RenderPresent(renderer);
}

void SDLRenderer::drawTile(uint32_t x, uint32_t y, uint8_t dir, TileType tile) {
    SDL_FRect dst;
    dst.x = x;
    dst.y = y;
    dst.w = tileSize;
    dst.h = tileSize;
    (void)dir; // Suppress unused parameter warning
    SDL_Texture* texture = nullptr;

    // Choose color based on tile
    switch (tile) {
        case TileType::Wall: // wall
            texture = seaweedTexture;
            break;

		case TileType::ExitClosed: // exit closed
            texture = exitClosedTexture;
            break;

		case TileType::ExitOpen: // exit open
            texture = exitOpenTexture;
            break;

		case TileType::Fish: {// fish
			texture = fishTexture[frame / 16 % 2]; // every 4 frames update
			break;
		}

		case TileType::Heart: {// heart
			texture = heartTextures[dir % 32]; // dir is fraction of life to show
			SDL_RenderTexture(renderer, texture, nullptr, &dst);
			return;
		}

        case TileType::Shark: { // shark 
            uint8_t frame_position = (x + y) % 16 / 4;
            texture = sharkTextures[dir % 4][frame_position];
			SDL_RenderTexture(renderer, texture, nullptr, &dst);
            return;
        }

        case TileType::Player: { // player 
            uint8_t frame_position = (x + y) % 16 / 4;
            texture = dolphinTextures[dir % 4][frame_position];
			SDL_RenderTexture(renderer, texture, nullptr, &dst);
            return;
        }


        default: // empty
            texture = waterTexture;
            break;
    }
    SDL_RenderTexture(renderer, waterTexture, nullptr, &dst);
    SDL_RenderTexture(renderer, texture, nullptr, &dst);
}

void SDLRenderer::getTextPos(int len, int optionPos, float &x, float &y) {
	switch (optionPos) {
		case 0: {
			x = 0.0f;
			y = 0.0f;
			break ;
		}
		case 1: {
			x = windowWidth / 2.0f - len * textHeight / 4.0f;
			y = 0.0f;
			break ;
		}
		case 2: {
			x = windowWidth / 2.0f - len * textHeight / 4.0f;
			y = textHeight * 2;
			break ;
		}
		case 20: {
			x = windowWidth / 2.0f - len * textHeight / 4.0f;
			y = textHeight;
			break ;
		}
		case 21: {
			x = 0;
			y = textHeight * 2;
			break ;
		}
		case 3: {
			x = windowWidth / 2.0f - len * textHeight / 4.0f;
			y = textHeight * 4;
			break ;
		}
		case 4: {
			x = windowWidth / 2.0f - len * textHeight / 4.0f;
			y = textHeight * 6;
			break ;
		}
		case 40: {
			x = 0;
			y = textHeight * 2;
			break ;
		}
		case 41: {
			x = 0;
			y = textHeight * 3;
			break ;
		}
		case 42: {
			x = 0;
			y = textHeight * 4;
			break ;
		}
		case 43: {
			x = 0;
			y = textHeight * 5;
			break ;
		}
		case 44: {
			x = 0;
			y = textHeight * 6;
			break ;
		}
		case 45: {
			x = 0;
			y = textHeight * 7;
			break ;
		}
		case 46: {
			x = 0;
			y = textHeight * 8;
			break ;
		}
		case 47: {
			x = 0;
			y = textHeight * 9;
			break ;
		}
		case 48: {
			x = 0;
			y = textHeight * 10;
			break ;
		}
		case 49: {
			x = 0;
			y = textHeight * 11;
			break ;
		}
		case 50: {
			x = 0;
			y = textHeight * 12;
			break ;
		}
		case 51: {
			x = 0;
			y = textHeight * 13;
			break ;
		}
		case 52: {
			x = 0;
			y = textHeight * 14;
			break ;
		}
		case 53: {
			x = 0;
			y = textHeight * 15;
			break ;
		}
		case 54: {
			x = 0;
			y = textHeight * 16;
			break ;
		}
		case 55: {
			x = 0;
			y = textHeight * 17;
			break ;
		}
		case 56: {
			x = 0;
			y = textHeight * 18;
			break ;
		}
		case 57: {
			x = 0;
			y = textHeight * 19;
			break ;
		}
		case 5: {
			x = windowWidth / 2.0f - len * textHeight / 4.0f;
			y = textHeight * 8;
			break ;
		}
		case 6: {
			x = windowWidth / 2.0f - len * textHeight / 4.0f;
			y = textHeight * 10;
			break ;
		}
		default: {
			x = windowWidth - len * textHeight / 2.0f;
			y = windowHeight - textHeight;
		}
	}
}

void SDLRenderer::getTextColor(int optionColor, SDL_Color &textColor, SDL_Color &bgColor) {
	switch (optionColor) {
		case 0: {
			textColor = {255, 255, 255, 255}; //rgba
			bgColor   = {80, 80, 80, 80};    //  grey transparent
			break ;
		}
		case 1: {
			textColor = {31, 127, 255, 255}; //rgba
			bgColor   = {7, 31, 63, 255};    //  grey transparent
			break ;
		}
		case 2: {
			textColor = {0, 0, 0, 255}; //rgba
			bgColor   = {0, 255, 0, 255};    //  grey transparent
			break ;
		}
		case 3: {
			textColor = {0, 255, 0, 255}; //rgba
			bgColor   = {0, 0, 0, 0};    //  no background
			break ;
		}
		case 4: {
			textColor = {80, 80, 80, 255}; //rgba
			bgColor   = {0, 0, 0, 0};    //  no background
			break ;
		}
		case 41: {
			textColor = {0, 255, 0, 255}; //rgba
			bgColor   = {0, 0, 0, 0};    //  no background
			break ;
		}
		case 42: {
			textColor = {0, 80, 0, 255}; //rgba
			bgColor   = {0, 0, 0, 0};    //  no background
			break ;
		}
		default: {
			textColor = {255, 255, 255, 255}; //rgba
			bgColor   = {0, 0, 0, 0};    //  no background
		}
	}
}

void SDLRenderer::drawText(const std::string& text, int optionColor, int optionPos) {
    if (!renderer || !fontSweet16mono || text.empty()) {
        return;
    }
	float x = 0.0f;
	float y = 0.0f;
	getTextPos(text.size(), optionPos, x, y);

    // Render the text to a surface.
    SDL_Color textColor = {255, 255, 255, 255}; //rgba
	SDL_Color bgColor   = {80, 80, 80, 80};    //  grey transparent
	getTextColor(optionColor, textColor, bgColor);

	SDL_Surface* surface = TTF_RenderText_Shaded(
		fontSweet16mono,
		text.c_str(),
		0,
		textColor,
		bgColor
	);
    // SDL_Surface* surface = TTF_RenderText_Blended(
    //     fontSweet16mono,
    //     text.c_str(),
    //     0,       // 0 means null-terminated string
    //     textColor
    // );

    if (!surface) {
        std::cerr << "TTF_RenderText_Blended failed: "
                  << SDL_GetError() << std::endl;
        return;
    }

    // Convert the surface into a texture.
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface failed: "
                  << SDL_GetError() << std::endl;
        SDL_DestroySurface(surface);
        return;
    }

    // Draw at the top-left corner (0, 0).
    SDL_FRect dst = {
        x,
        y,
        static_cast<float>(surface->w),
        static_cast<float>(surface->h)
    };

    SDL_RenderTexture(renderer, texture, nullptr, &dst);

    // Free temporary resources.
    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}

void SDLRenderer::shutdown() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

	if (ttfInitialized) {
        TTF_Quit();
        ttfInitialized = false;
		fontSweet16mono = nullptr;
    }

    if (fishTexture[0]) {
        SDL_DestroyTexture(fishTexture[0]);
        fishTexture[0] = nullptr;
    }
	 if (fishTexture[1]) {
        SDL_DestroyTexture(fishTexture[1]);
        fishTexture[1] = nullptr;
    }

    if (sharkTargetTexture) {
        SDL_DestroyTexture(sharkTargetTexture);
        sharkTargetTexture = nullptr;
    }

	if (waterTexture) {
        SDL_DestroyTexture(waterTexture);
        waterTexture = nullptr;
    }
	if (seaweedTexture) {
        SDL_DestroyTexture(seaweedTexture);
        seaweedTexture = nullptr;
    }
	if (wallTexture) {
        SDL_DestroyTexture(wallTexture);
        wallTexture = nullptr;
    }
	if (exitOpenTexture) {
        SDL_DestroyTexture(exitOpenTexture);
        exitOpenTexture = nullptr;
    }
	if (exitClosedTexture) {
        SDL_DestroyTexture(exitClosedTexture);
        exitClosedTexture = nullptr;
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 1; j++) {
            if (dolphinTextures[i][j]) {
                SDL_DestroyTexture(dolphinTextures[i][j]);
                dolphinTextures[i][j] = nullptr;
            }
			if (sharkTextures[i][j]) {
                SDL_DestroyTexture(sharkTextures[i][j]);
                sharkTextures[i][j] = nullptr;
            }
        }
    }
	for (int i = 0; i < 32; i++) {
		if (heartTextures[i]) {
			SDL_DestroyTexture(heartTextures[i]);
			heartTextures[i] = nullptr;
		}
	}
}