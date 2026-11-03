#include "AtfatfGame.hpp"
#include "map/MapEditor.hpp"
#include "render/IRenderer.hpp"
#include "logger.hpp"
#include "getTime.hpp"

#include <format>
#include <thread>
#include <chrono>

#include <algorithm>

// static bool overlaps(
//     int x1, int y1, int tileSize1,
//     int x2, int y2, int tileSize2)
// {
//     return x1 < x2 + tileSize2 &&
//            x1 + tileSize1 > x2 &&
//            y1 < y2 + tileSize2 &&
//            y1 + tileSize1 > y2;
// }

int AtfatfGame::getLevel() {
	return level;
}

int AtfatfGame::getLives() {
	return lives;
}


int AtfatfGame::getScore() {
	return score;
}

int AtfatfGame::getTimeLeft() {
	return timeLeft;
}
static int distanceSquared(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    return dx * dx + dy * dy;
}

static bool overlapsEnough(
    int x1, int y1, int tileSize1,
    int x2, int y2, int tileSize2)
{
    const int minOverlap = (tileSize1 + 3) / 4;

    const int overlapX =
        std::min(x1 + tileSize1, x2 + tileSize2) -
        std::max(x1, x2);

    const int overlapY =
        std::min(y1 + tileSize1, y2 + tileSize2) -
        std::max(y1, y2);

    return overlapX >= minOverlap &&
           overlapY >= minOverlap;
}


void AtfatfGame::init(const std::string& mapPath) {
    MapEditor editor(map);
    editor.buildFromPath(mapPath);
	level = 1;
	playerDir = 0;
	playerNextDir = 0;
	isMoving = false;
	timeLeft = 30000;
	lastTime = getTimeInMilliseconds();
    // find spawn, exit and stuff
    for (uint32_t y = 0; y < map.height(); y++) {
        for (uint32_t x = 0; x < map.width(); x++) {
            if (map.get(x, y) == TileType::PlayerSpawn) { // TODO: rename to getTile
                playerPos = {(int)x * tileSize, (int)y * tileSize};
            }
            if (map.get(x, y) == TileType::SharkSpawn) {
				sharksDir[sharksTotal] = 2; //right
                sharksPos[sharksTotal] = {(int)x * tileSize, (int)y * tileSize};
				sharksType[sharksTotal] = SharkType::Hunter;
				sharksStatus[sharksTotal] = SharkStatus::Chase;
				++sharksTotal;
            }
			if (map.get(x, y) == TileType::Fish)
				fishTotal++; 
        }
    }

    events.push_back(AtfatfEvent::GAME_STARTED);
}

void AtfatfGame::nextLevel(void) {
    MapEditor editor(map);
	std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	std::string mapPath = std::format("./assets/maps/level{:02}.map", level);
	++level;
	lives += 32;
	timeLeft += 30000;
	playerDir = 0;
	playerNextDir = 0;
	isMoving = false;
	lastTime = getTimeInMilliseconds();
    editor.buildFromPath(mapPath);
	sharksTotal = 0;
	fishTotal = 0;
    // find spawn, exit and stuff
    for (uint32_t y = 0; y < map.height(); y++) {
        for (uint32_t x = 0; x < map.width(); x++) {
            if (map.get(x, y) == TileType::PlayerSpawn) { // TODO: rename to getTile
                playerPos = {(int)x * tileSize, (int)y * tileSize};
				playerDir = 0;
            }
            if (map.get(x, y) == TileType::SharkSpawn) {
				sharksDir[sharksTotal] = 2; //right
                sharksPos[sharksTotal] = {(int)x * tileSize, (int)y * tileSize};
				sharksType[sharksTotal] = SharkType::Hunter;
				sharksStatus[sharksTotal] = SharkStatus::Chase;
				++sharksTotal;
            }
			if (map.get(x, y) == TileType::Fish)
				fishTotal++; 
        }
    }
	playerDir = 0;
	isMoving = false;
    events.push_back(AtfatfEvent::GAME_STARTED);
}

void AtfatfGame::sameLevel(void) {
    MapEditor editor(map);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	--level;
	std::string mapPath = std::format("./assets/maps/level{:02}.map", level);
	++level;
	lastTime = getTimeInMilliseconds();
	playerDir = 0;
	playerNextDir = 0;
	isMoving = false;
    editor.buildFromPath(mapPath);
	sharksTotal = 0;
	fishTotal = 0;
    // find spawn, exit and stuff
    for (uint32_t y = 0; y < map.height(); y++) {
        for (uint32_t x = 0; x < map.width(); x++) {
            if (map.get(x, y) == TileType::PlayerSpawn) { // TODO: rename to getTile
                playerPos = {(int)x * tileSize, (int)y * tileSize};
				playerDir = 0;
            }
            if (map.get(x, y) == TileType::SharkSpawn) {
				sharksDir[sharksTotal] = 2; //right
                sharksPos[sharksTotal] = {(int)x * tileSize, (int)y * tileSize};
				sharksType[sharksTotal] = SharkType::Hunter;
				sharksStatus[sharksTotal] = SharkStatus::Chase;
				++sharksTotal;
            }
			if (map.get(x, y) == TileType::Fish)
				fishTotal++; 
        }
    }

    events.push_back(AtfatfEvent::GAME_RESTARTED);
}

void AtfatfGame::handleInput(InputKey input) {
    switch (input) {
        case InputKey::UP:
		case InputKey::W: {
			playerNextDir = 3;
			isMoving = true;
			break;
		}
        case InputKey::DOWN: 
		case InputKey::S: {
			playerNextDir = 1;
			isMoving = true;
			break;
		}
        case InputKey::LEFT: 
		case InputKey::A: {
			playerNextDir = 2;
			isMoving = true;
			break;
		}
        case InputKey::RIGHT: 
		case InputKey::D: {
			playerNextDir = 0;
			isMoving = true;
			break;
		}
		case InputKey::SPACE: {
			isMoving ^= true;
			break;
		}
		case InputKey::F1: { //cheat
			timeLeft += 30000;
			lives += 32;
			break;
		}
		case InputKey::F2: { //cheat
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			sameLevel();
			break;
		}
		case InputKey::F3: { //cheat
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			nextLevel();
			break;
		}
        default: break;
    }
}

void AtfatfGame::moveShark(int shark) {

	int dx = 0;
    int dy = 0;

	// if crossroad 3 update dir
	// if crossroad 4
	// if deadend turn around
	//
	if (sharksPos[shark].x % tileSize == 0 && sharksPos[shark].y % tileSize == 0) {
		// check front, left and right
		// FRONT
		int dfx = 0;
    	int dfy = 0;
		switch (sharksDir[shark]) {
			case 0: dfx = sharkSpeed; break; // right
			case 1: dfy = sharkSpeed; break; // down
			case 2: dfx = -sharkSpeed; break; // left
			case 3: dfy = -sharkSpeed; break; // up
		}
		// front
		int fx = sharksPos[shark].x + tileSize * dfx / sharkSpeed;
		int fy = sharksPos[shark].y + tileSize * dfy / sharkSpeed;

		// RIGHT
		int drx = 0;
    	int dry = 0;
		switch ((sharksDir[shark] + 1) % 4) {
			case 0: drx = sharkSpeed; break; // right
			case 1: dry = sharkSpeed; break; // down
			case 2: drx = -sharkSpeed; break; // left
			case 3: dry = -sharkSpeed; break; // up
		}
		int rx = sharksPos[shark].x + tileSize * drx / sharkSpeed;
		int ry = sharksPos[shark].y + tileSize * dry / sharkSpeed;

		// LEFT
		int dlx = 0;
    	int dly = 0;
		switch ((sharksDir[shark] + 3) % 4) {
			case 0: dlx = sharkSpeed; break; // right
			case 1: dly = sharkSpeed; break; // down
			case 2: dlx = -sharkSpeed; break; // left
			case 3: dly = -sharkSpeed; break; // up
		}
		int lx = sharksPos[shark].x + tileSize * dlx / sharkSpeed;
		int ly = sharksPos[shark].y + tileSize * dly / sharkSpeed;
		
		// BACK
		int dbx = 0;
    	int dby = 0;
		switch ((sharksDir[shark] + 2) % 4) {
			case 0: dbx = sharkSpeed; break; // right
			case 1: dby = sharkSpeed; break; // down
			case 2: dbx = -sharkSpeed; break; // left
			case 3: dby = -sharkSpeed; break; // up
		}
		int bx = sharksPos[shark].x + tileSize * dbx / sharkSpeed;
		int by = sharksPos[shark].y + tileSize * dby / sharkSpeed;
		bool isFrontFree = map.get(fx / tileSize, fy / tileSize) != TileType::Wall;
		bool isRightFree = map.get(rx / tileSize, ry / tileSize) != TileType::Wall;
		bool isLeftFree = map.get(lx / tileSize, ly / tileSize) != TileType::Wall;
		bool isBackFree = map.get(bx / tileSize, by / tileSize) != TileType::Wall;
		int paths = isFrontFree + isRightFree + isLeftFree;
		if (paths == 0 && isBackFree) {
			sharksDir[shark] = (sharksDir[shark] + 2) % 4;
			sharksPos[shark].x += dbx;
			sharksPos[shark].y += dby;
		}
		else if (paths == 1) {
			if (isFrontFree) {
				// continue
				sharksPos[shark].x += dfx;
				sharksPos[shark].y += dfy;
			}
			else if (isRightFree) {
				// turn right
				sharksDir[shark] = (sharksDir[shark] + 1) % 4;
				sharksPos[shark].x += drx;
				sharksPos[shark].y += dry;
			}
			else {
				// turn left
				sharksDir[shark] = (sharksDir[shark] + 3) % 4;
				sharksPos[shark].x += dlx;
				sharksPos[shark].y += dly;
			}
		}
		else if (paths > 1) {
			int ox = 0;
			int oy = 0;
			// switch ((playerDir + shark) % 4) {
			// 	case 0: ox = sharkSpeed * shark * 8; break;
			// 	case 1: oy = sharkSpeed * shark * 8; break;
			// 	case 2: ox = -sharkSpeed * shark * 8; break;
			// 	case 3: oy = -sharkSpeed * shark * 8; break;
			// }
			int distanceFront = distanceSquared(playerPos.x + ox, playerPos.y + oy, sharksPos[shark].x + dfx, sharksPos[shark].y + dfy);
			int	distanceRight = distanceSquared(playerPos.x + ox, playerPos.y + oy, sharksPos[shark].x + drx, sharksPos[shark].y + dry);
			int distanceLeft = distanceSquared(playerPos.x + ox, playerPos.y + oy, sharksPos[shark].x + dlx, sharksPos[shark].y + dly);
			if (distanceFront <= distanceRight && distanceFront <= distanceLeft && isFrontFree) {
				sharksPos[shark].x += dfx;
				sharksPos[shark].y += dfy;
			}
			else if (distanceRight <= distanceLeft && isRightFree) {
				sharksDir[shark] = (sharksDir[shark] + 1) % 4;
				sharksPos[shark].x += drx;
				sharksPos[shark].y += dry;
			}
			else if (isLeftFree) {
				sharksDir[shark] = (sharksDir[shark] + 3) % 4;
				sharksPos[shark].x += dlx;
				sharksPos[shark].y += dly;
			}
			else if (isFrontFree) {
				sharksPos[shark].x += dfx;
				sharksPos[shark].y += dfy;
			}
			// calculate target and move that way
		}

	}
	else {
		switch (sharksDir[shark]) {
			case 0: dx = sharkSpeed; break; // right
			case 1: dy = sharkSpeed; break; // down
			case 2: dx = -sharkSpeed; break; // left
			case 3: dy = -sharkSpeed; break; // up
		}
		sharksPos[shark].x += dx;
		sharksPos[shark].y += dy;
    }

	// teleport through tunnels
	if (sharksPos[shark].x < sharkSpeed)
		sharksPos[shark].x = ((map.width()) * tileSize) - tileSize * 2;
	else if ((uint32_t)sharksPos[shark].x >= (((map.width()) * tileSize) - tileSize ))
		sharksPos[shark].x = tileSize;
	if (sharksPos[shark].y < sharkSpeed)
		sharksPos[shark].y = ((map.height()) * tileSize) - tileSize * 2;
	else if ((uint32_t)sharksPos[shark].y >= (((map.height()) * tileSize) - tileSize ))
		sharksPos[shark].y = tileSize;

	// attack player
	if (overlapsEnough(playerPos.x, playerPos.y, tileSize, sharksPos[shark].x, sharksPos[shark].y, tileSize)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (timeLeft >= 250)
			timeLeft -= 250;
		else
			timeLeft = 0;
		if (score > 10)
			score -= 10;
		else
			score = 0;
		if (lives % 32 != 1)
			--lives;
		else if (lives > 1) {
			--lives;
			events.push_back(AtfatfEvent::LEVEL_FAILED);
			sameLevel();
		}
		else {
			events.push_back(AtfatfEvent::GAME_OVER);
		}
	}
}

void AtfatfGame::moveSharks(void) {
    for (int i = 0; i < sharksTotal; i++) {
        // Move each ghost based on its current status
        moveShark(i);
    }
}

void AtfatfGame::movePlayer(void) {
	if (isMoving == false)
		return;
    int dx = 0;
    int dy = 0;
	
    // try to move in new direction if possible
    switch (playerNextDir) {
        case 0: dx = playerSpeed; break; // right
        case 1: dy = playerSpeed; break; // down
        case 2: dx = -playerSpeed; break; // left
        case 3: dy = -playerSpeed; break; // up
    }
    int nx = playerPos.x + tileSize * dx / playerSpeed;
    int ny = playerPos.y + tileSize * dy / playerSpeed;
    if (map.get(nx / tileSize, ny / tileSize) != TileType::Wall && playerPos.x % tileSize == 0 && playerPos.y % tileSize == 0) {
        playerDir = playerNextDir;
    }
	dx = 0;
    dy = 0;
    // std::cout << "Handling player movement..." << std::endl;
    // std::cout << "Player direction: " << playerDir << ", next direction: " << playerNextDir << std::endl;
    // std::cout << "Player position: (" << playerPos.x << ", " << playerPos.y << ")" << std::endl;

    switch (playerDir) {
        case 0: dx = playerSpeed; break; // right
        case 1: dy = playerSpeed; break; // down
        case 2: dx = -playerSpeed; break; // left
        case 3: dy = -playerSpeed; break; // up
    }
    //try to move in old direction
    nx = playerPos.x + tileSize * dx / playerSpeed;
	ny = playerPos.y + tileSize * dy / playerSpeed;
    //check if new tile is not wall
    if (playerPos.x % tileSize || playerPos.y % tileSize || map.get(nx / tileSize, ny / tileSize) != TileType::Wall) {
        if (map.get(nx / tileSize, ny / tileSize) == TileType::Fish && playerPos.x % tileSize == 0 && playerPos.y % tileSize == 0) {
            score += (timeLeft / 1000) * level;
            events.push_back(AtfatfEvent::FISH_EATEN);
            map.set(nx / tileSize, ny / tileSize, TileType::Empty);
			fishTotal--;
			timeLeft += 500;
			if (lives % 32 != 0)
				++lives;
        }
        playerPos.x += dx;
        playerPos.y += dy;
		if (playerPos.x % tileSize == 0 && playerPos.y % tileSize == 0)
			++steps;
        events.push_back(AtfatfEvent::PLAYER_MOVED);
		moveSharks();
    }
	if (playerPos.x % tileSize == 0 && playerPos.y % tileSize == 0 && map.get(playerPos.x / tileSize, playerPos.y / tileSize) == TileType::Exit && fishTotal == 0) {
		events.push_back(AtfatfEvent::LEVEL_COMPLETED);
		score += timeLeft * level / 100;
		nextLevel();
	}
    // teleport through tunnels
    if (playerPos.x < playerSpeed)
        playerPos.x = ((map.width()) * tileSize) - tileSize * 2;
    else if ((uint32_t)playerPos.x >= (((map.width()) * tileSize) - tileSize ))
        playerPos.x = tileSize;
    if (playerPos.y < playerSpeed)
        playerPos.y = ((map.height()) * tileSize) - tileSize * 2;
    else if ((uint32_t)playerPos.y >= (((map.height()) * tileSize) - tileSize ))
        playerPos.y = tileSize;

    
}

void AtfatfGame::update(double) {
	frame++;
    movePlayer();
	if (timeLeft == 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		events.push_back(AtfatfEvent::GAME_OVER);
	}
    // moveSharks();
    // push events?
}

void AtfatfGame::render(IRenderer& renderer) {
	LOG_VERBOSE("In renderer TimeLeft: {} Level: {} Lives: {}", timeLeft, level, lives);
	uint64_t now = getTimeInMilliseconds();
	uint64_t timeDiff = now - lastTime;
	lastTime = now;
	if (timeLeft >= timeDiff){
		timeLeft -= timeDiff;
	}
	else {
		timeLeft = 0;
		// endGame
	}
    static int frameCount = 0;
    frameCount++;
    uint8_t dir = 0;
    for (uint32_t y = 0; y < map.height(); y++) {
        for (uint32_t x = 0; x < map.width(); x++) {
            dir = 0;
            TileType tile = map.get(x, y);
            if (tile == TileType::PowerPellet) {
                dir = (frameCount / 16) % 2; // Alternate between two textures for animation
            }
			if (tile == TileType::Exit) {
                if (fishTotal > 0)
					tile = TileType::ExitClosed;
				else
					tile = TileType::ExitOpen;
            }
            renderer.drawTile(x * tileSize, y * tileSize, dir, tile);
        }
    }
    renderer.drawTile(playerPos.x, playerPos.y, playerDir, TileType::Player);
	for (int i = 0; i < sharksTotal; ++i) {
		renderer.drawTile(sharksPos[i].x, sharksPos[i].y, sharksDir[i], TileType::Shark);
	}
	for (int i = 0; i < lives; i += 32) {
		if (i + 32 >= lives)
			renderer.drawTile((60 * i) / 32, 1080 - 60, lives % 32, TileType::Heart);
		else
			renderer.drawTile((60 * i) / 32, 1080 - 60, 0, TileType::Heart);
	}
	
    // draw ghosts, tunnel...
}
