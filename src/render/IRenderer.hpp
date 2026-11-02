#pragma once

#include <string>
#include "TileType.hpp"

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void clear() = 0;
    virtual void present() = 0;

    virtual void drawTile(uint32_t x, uint32_t y, uint8_t dir, TileType tile) = 0;
    virtual void drawText(const std::string& text, int optionColor = 0, int optionPos = 0) = 0;
};