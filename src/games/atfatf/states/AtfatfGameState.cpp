
#include "AtfatfGameState.hpp"
#include "AtfatfMenuState.hpp"
#include "engine/Engine.hpp"
#include "logger.hpp"

#include "GameVersion.hpp"

#include <format>

AtfatfGameState::AtfatfGameState(Engine& eng, std::string name, int coalitionID, std::string campus) : engine(eng), name(name), coalitionID(coalitionID), campus(campus) {
    game.init("assets/maps/level00.map");
}

#include <chrono>
#include <format>
#include <fstream>
#include <string>
#include <stdexcept>
#include <filesystem>

void AtfatfGameState::addScore(const std::string& line)
{

    const auto today = floor<std::chrono::days>(std::chrono::system_clock::now());
    const std::chrono::year_month_day date{today};
	std::error_code ec;
	std::string scores = "scores";
	std::filesystem::create_directories(scores, ec);

	if (ec) {
		throw std::runtime_error(
			"Failed to create scores directory: " + ec.message()
		);
	}

	
	std::string version = std::format("{}/{}.{}.{}", scores, GameVersion::Major, GameVersion::Minor, GameVersion::Patch);
	std::filesystem::create_directories(version, ec);

	if (ec) {
		throw std::runtime_error(
			"Failed to create scores directory: " + ec.message()
		);
	}
    const std::string filename = std::format(
        "{}/{:04}_{:02}_{:02}.score",
		version,
        int(date.year()),
        unsigned(date.month()),
        unsigned(date.day())
    );

    std::ofstream file(filename, std::ios::app);

    if (!file)
        throw std::runtime_error("Failed to open score file: " + filename);

    file << line << '\n';
}

void	AtfatfGameState::endGame() {
	const auto now = floor<std::chrono::seconds>(std::chrono::system_clock::now());
	const auto today = floor<std::chrono::days>(now);
    const std::chrono::hh_mm_ss time{floor<std::chrono::seconds>(now - today)};
	std::string line = std::format("{:02}:{:02}:{:02}:{}:{}:{}:{:02}:{:010}",
		time.hours().count(),
        time.minutes().count(),
        time.seconds().count(),
		name, campus, coalitions[coalitionID],
		game.getLevel(),
		game.getScore());
	LOG_NOTICE("RESULT: {}", line);
	addScore(line);
	engine.setState(std::make_unique<AtfatfMenuState>(engine));
}

void AtfatfGameState::handleInput(InputKey input) {
    if (input == InputKey::ESCAPE) {
		endGame();
        return;
    }
	if (input == InputKey::QUIT) {
		endGame();
        engine.stop();
		return;
    }
    game.handleInput(input);
}

void AtfatfGameState::update(double dt) {
    game.update(dt);
    for (auto event : game.events) {
        engine.getAudio().playEvent(event);
		if (event == AtfatfEvent::GAME_OVER) {
			endGame();
		}
    }
    game.events.clear();
}

void AtfatfGameState::render(IRenderer& renderer) {
    game.render(renderer);
	std::string report = std::format("level:{:2}, Time left:{:3}, Score{:8}, Lives:{:2}.{:02} {} {} {}", game.getLevel(), game.getTimeLeft() / 1000, game.getScore(),  game.getLives() / 32, game.getLives() % 32 * 3, name, coalitions[coalitionID], campus);
	renderer.drawText(report);
}