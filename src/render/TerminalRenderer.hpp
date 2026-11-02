#pragma once

#include "IRenderer.hpp"

class TerminalRenderer : public IRenderer
{
public:
    void clear() override;
    void present() override;
    void drawTile(uint32_t x, uint32_t y, uint8_t dir, TileType tile) override;
    // void drawMap(const Map& map) override;
    void drawText(const std::string& text, int optionColor = 0, int optionPos = 0) override;
};
