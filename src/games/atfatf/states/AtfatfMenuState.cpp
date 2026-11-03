#include "AtfatfMenuState.hpp"
#include "AtfatfNameState.hpp"
#include "AtfatfHighscoreState.hpp"
#include "AtfatfGameState.hpp"
#include "engine/Engine.hpp"
#include "logger.hpp"

#include <algorithm>
#include <iterator>

AtfatfMenuState::AtfatfMenuState(Engine& eng) : engine(eng) {
    engine.getAudio().playMusic("menu", -1);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, quotes.size() - 1);
	quoteID = dist(gen);
}

void AtfatfMenuState::moveSelection(int direction)
{
    auto it = std::find(
        std::begin(menuButtons),
        std::end(menuButtons),
        activeButton
    );

    if (it == std::end(menuButtons))
        return;

    if (direction > 0 && std::next(it) != std::end(menuButtons))
        activeButton = *std::next(it);
    else if (direction < 0 && it != std::begin(menuButtons))
        activeButton = *std::prev(it);
}

void AtfatfMenuState::handleInput(InputKey input) {
    if (input == InputKey::ENTER) {
		if (activeButton == ButtonSelected::Tournament) {
			engine.getAudio().stopMusic();
			engine.setState(std::make_unique<AtfatfNameState>(engine));
		}
		else if (activeButton == ButtonSelected::Highscore) {
			engine.setState(std::make_unique<AtfatfHighscoreState>(engine));
		}
		else if (activeButton == ButtonSelected::Exit) {
			engine.stop();
		}
    }
    else if (input == InputKey::QUIT) {
        engine.stop();
    }
	else if (input == InputKey::DOWN) {
		moveSelection(1);
    }
	else if (input == InputKey::UP) {
		moveSelection(-1);
    }
}

void AtfatfMenuState::update(double) {}

void AtfatfMenuState::render(IRenderer& renderer) {
	LOG_TRACE("in menu loop");
    renderer.drawText(" And Thanks For All The Fish ", 1, 1);
	renderer.drawText(" New Game ", 4, 2);
	if (activeButton == ButtonSelected::Tournament)
		renderer.drawText(" Tournament ", 2, 3);
	else
		renderer.drawText(" Tournament ", 3, 3);
	if (activeButton == ButtonSelected::Highscore)
		renderer.drawText(" Highscore ", 2, 4);
	else
		renderer.drawText(" Highscore ", 3, 4);
	renderer.drawText(" Settings ", 4, 5);
	if (activeButton == ButtonSelected::Exit)
		renderer.drawText(" Exit ", 2, 6);
	else
		renderer.drawText(" Exit ", 3, 6);
	
	renderer.drawText(quotes[quoteID], 4, 7);
	// draw button
}