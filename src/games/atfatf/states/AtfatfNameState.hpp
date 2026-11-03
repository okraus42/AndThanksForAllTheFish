#pragma once
#include "engine/IGameState.hpp"

#include <random>
#include <string>
#include <vector>

class Engine;

class AtfatfNameState : public IGameState {
private:
    Engine& engine;
	std::string name = "";
	std::string campus = "42 Prague";
	std::vector<std::string> coalitions = {
		"Dolphins",
		"Sharks",
		"Aurora",
		"Eclipse",
		"Supernova"
	};
	int coalitionID;

    void moveSelection(int direction);

public:
    AtfatfNameState(Engine& eng);

    void handleInput(InputKey input) override;
    void update(double dt) override;
    void render(IRenderer& renderer) override;
};