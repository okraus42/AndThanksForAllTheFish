#pragma once

#include <vector>
#include "map/Map.hpp"
#include "input/InputKey.hpp"
#include "render/IRenderer.hpp"
#include "AtfatfEvent.hpp"

struct Vec2 {
    int x;
    int y;
};

// 0 Blinky, 1 Pinky, 2 Inky, 3 Clyde
enum class SharkType {
    Hunter,
    Shy,
    Chaser,
    Smart
};

enum class SharkStatus {
    Chase,
    Scatter,
    Scared,
    Eaten,
    Captured
};

// map 256 * 256
// player 256
// fish 256
// enemy 256
// exit 256

class AtfatfGame {
public:
    


// time

    std::vector<AtfatfEvent> events;

    void init(const std::string& mapPath);

    void handleInput(InputKey input);
    void update(double dt);
    void render(IRenderer& renderer);
	int	getLevel(void);
	int	getLives(void);
	int	getScore(void);
	int	getTimeLeft(void);

private:
    void movePlayer(void);
    void moveSharks(void);
    void moveShark(int shark);
	void nextLevel(void);
	void sameLevel(void);

	Map map;

    Vec2 playerPos{};
    Vec2 sharksPos[256]{};
    // Vec2 sharksTarget[256]{};
    int sharksDir[256]{}; // 0: right, 1: down, 2: left, 3: up
    SharkStatus sharksStatus[256]{};
	SharkType sharksType[256]{};
	int sharksTotal = 0;
    int playerDir = 0; // 0: right, 1: down, 2: left, 3: up
    int playerNextDir = 0;
    int playerSpeed = 6; // pixels per frame
	int sharkSpeed = 4; // pixels per frame
    int score = 0;
    int tileSize = 60;
	int fishTotal = 0;
	int level = 0;
	const int levels = 10;
	int lives = 32; // 32 is one life
	uint64_t lastTime;
	uint64_t timeLeft = 60000; //ms
	unsigned int frame = 0;
	unsigned int steps = 0;
	bool isMoving = false;
	uint64_t timeFreeze = 0; //ms
	AtfatfEvent freezeEvent;
	int timeWarnings = 5;

	std::string name = "tester";
	std::string coalition = "Aurora";
	std::string campus = "42 Prague";
};