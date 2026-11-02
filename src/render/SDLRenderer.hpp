#pragma once

#include "IRenderer.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

class SDLRenderer : public IRenderer
{
public:
    SDLRenderer();
    ~SDLRenderer();

    bool init();
    void shutdown();

    void clear() override;
    void present() override;
    void drawTile(uint32_t x, uint32_t y, uint8_t dir, TileType tile) override;
    void drawText(const std::string& text,int optionColor = 0, int optionPos = 0) override;

private:
	void getTextPos(int len, int optionPos, float &x, float &y);
	void getTextColor(int optionColor, SDL_Color &textColor, SDL_Color &bgColor);


	int windowWidth;
	int windowHeight;
	float textHeight;
    SDL_Window* window;
    SDL_Renderer* renderer;

	uint64_t	frame;

    bool loadTexture(const std::string& path, SDL_Texture*& outTexture);
    SDL_Texture* fishTexture[2];
	SDL_Texture* heartTextures[32];
    SDL_Texture* exitOpenTexture;
	SDL_Texture* exitClosedTexture;
	SDL_Texture* wallTexture;
	SDL_Texture* waterTexture;
	SDL_Texture* seaweedTexture;
    // SDL_Texture* wallTextures[12];
    SDL_Texture* dolphinTextures[4][4]; // 4 directions, 4 frames each
    SDL_Texture* sharkTextures[4][4]; // 4 directions, 2 frames + blue + white
    SDL_Texture* sharkTargetTexture;

	bool ttfInitialized = false;
	TTF_Font* fontSweet16mono = nullptr;


	// sprites and fonts would go here, but we don't have any yet
    // atfatf 4 directions, 3 frames each = 12 sprites
    // 4 ghosts, 4 directions, 2 frames each = 32 sprites
    // ghost eyes, 4 directions = 4 sprites
    // frightened ghost, 2 frames = 2 sprites
    // white ghost, 2 frames = 2 sprites
    // pellets, powerpellets, fruits
    // ghost targets, atfatf target
    // walls
    // paths
    // fonts (score, lives, level, etc.)
    // Ready msg, point sprites (100, 200, 400, 800, 1600...)
    int tileSize = 16; //not used, initialised in constructor
};