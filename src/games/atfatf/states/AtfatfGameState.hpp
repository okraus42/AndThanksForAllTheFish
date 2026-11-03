
#pragma once
#include "engine/IGameState.hpp"
#include "games/atfatf/AtfatfGame.hpp"

class Engine;

class AtfatfGameState : public IGameState {
private:
    Engine& engine;
    AtfatfGame game;
	std::string name;
	std::vector<std::string> coalitions = {
		"Dolphins",
		"Sharks",
		"Aurora",
		"Eclipse",
		"Supernova"
	};
	int coalitionID = 0;
	std::string campus;

	void	addScore(const std::string& line);
	void	endGame();

public:
    AtfatfGameState(Engine& eng, std::string name, int coalitionID, std::string campus);

    void handleInput(InputKey input) override;
    void update(double dt) override;
    void render(IRenderer& renderer) override;
};