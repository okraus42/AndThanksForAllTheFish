#include "map/MapEditor.hpp"
#include "logger.hpp"
#include <cstdint>
#include <fstream>

#include <iostream>

MapEditor::MapEditor(Map& m)
    : map(m)
{}

void MapEditor::setTile(std::uint32_t x, std::uint32_t y, TileType type)
{
    map.tiles[y * map.m_width + x] = type;
}

TileType MapEditor::getTile(std::uint32_t x, std::uint32_t y) const
{
    return map.tiles[y * map.m_width + x];
}

void MapEditor::buildFromMaze(const Maze& maze)
{
    for (std::uint32_t y = 0U; y < map.m_height; y++)
    {
        for (std::uint32_t x = 0U; x < map.m_width; x++)
        {
            setTile(x, y, TileType::Wall);
        }
    }

    for (std::uint32_t y = 0U; y < maze.height(); y++)
    {
        for (std::uint32_t x = 0U; x < maze.width(); x++)
        {
            std::uint32_t mx = x * 2U + 1U;
            std::uint32_t my = y * 2U + 1U;

            setTile(mx, my, TileType::Pellet);
        }
    }
}

void MapEditor::buildFromPath(const std::string& path) {

    std::ifstream file(path);
	LOG_NOTICE("Building map: {}", path);
    if (!file.is_open()) {
		LOG_ERROR("Failed to open map file {}", path);
        throw std::runtime_error("Failed to open map file");
    }

    std::string line;
    std::uint32_t y = 0U;
    map.m_width = 0U;
	map.m_height = 0U;
	map.tiles.clear();

    while (std::getline(file, line)) {
        if (!map.height()) {
            map.m_width = line.length();
        }
        else if (map.width() != line.length()) {
			LOG_ERROR("Inconsistent line lengths in map file {}\n map {} != {} (line)\n{}", path, map.width(), line.length(), line);
            throw std::runtime_error("Inconsistent line lengths in map file");
        }
        map.m_height++;
        map.tiles.resize(map.m_width * map.m_height, TileType::Empty);
        for (std::uint32_t x = 0U; x < line.length(); ++x) {
            char tile = line[x];
            switch (tile) {
                case ' ':
                    setTile(x, y, TileType::Empty);
                    break;
                case '#':
                    setTile(x, y, TileType::Wall);
                    break;
				case '0':
                    setTile(x, y, TileType::Empty);
                    break;
                case '1':
                    setTile(x, y, TileType::Wall);
                    break;
                case 'P':
                    setTile(x, y, TileType::PlayerSpawn);
                    break;
                case 'E':
                    setTile(x, y, TileType::Exit);
                    break;
                case 'C':
                    setTile(x, y, TileType::Fish);
                    break;
                case 'S':
                    setTile(x, y, TileType::SharkSpawn);
                    break;
                default:
                    setTile(x, y, TileType::Wall);
                    break;
            }
        }
        ++y;
    }

}