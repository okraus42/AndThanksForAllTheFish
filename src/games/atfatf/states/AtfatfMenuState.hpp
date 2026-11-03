#pragma once
#include "engine/IGameState.hpp"

#include <random>
#include <string>
#include <vector>

class Engine;

class AtfatfMenuState : public IGameState {
private:
    Engine& engine;

	enum class ButtonSelected {
		// NewGame,
        Tournament,
        Highscore,
		// Settings,
        Exit
    };

    static constexpr ButtonSelected menuButtons[] = {
        ButtonSelected::Tournament,
        ButtonSelected::Highscore,
        ButtonSelected::Exit
    };

    ButtonSelected activeButton = ButtonSelected::Tournament;



	std::vector<std::string> quotes = {
		"Welcome back!",
		"Ready to play?",
		"Good luck!",
		"Let's do this!",
		"Mostly harmless.",
		"Don't panic!",
		"42",
		"So long, and thanks for all the fish!",
		"Time is an illusion. Lunchtime doubly so.",
		"Bless the Maker and His water.",
		"The spice must flow.",
		"He who controls the spice controls the universe.",
		"There is no spoon.",
		"Follow the white rabbit.",
		"The cake is a lie.",
		"Winter is coming.",
		"Not all those who wander are lost.",
		"You shall not pass!",
		"One does not simply walk into Mordor.",
		"Do. Or do not. There is no try.",
		"May the Force be with you.",
		"Why so serious?",
		"Just keep swimming.",
		"Houston, we have a problem.",
		"Wind's howling.",
		"The single raindrop never feels responsible for the flood.",
		"Reality is hopelessly inaccurate.",
		"The day you stop learning is the day you begin decaying.",
		"One is never alone with a rubber duck.",
		"In order to fly, all one must do is simply miss the ground.",
		"The Guide is definitive. Reality is frequently inaccurate.",
		"Time doesn't necessarily happen in chronological order.",
		"It takes an awful lot of time to not write a book.",
		"Alea iacta ist.",
		"Veni, vidi, vici.",
		"Tempus fugit.",
		"Romani ite domum.",
		"Divide and conquere.",
		"Carpe diem!",
		"Carpe noctem!",
		"Draco dormiens nunquam titillandus.",
		"Ad astra per aspera.",
		"Failure is success in progress.",
		"Thinking is hard work; that's why so few do it.",
		"You never fail until you stop trying.",
		"Never give up, never surrender!",
		"Live long, and prosper."
	};
	int quoteID;

    void moveSelection(int direction);

public:
    AtfatfMenuState(Engine& eng);

    void handleInput(InputKey input) override;
    void update(double dt) override;
    void render(IRenderer& renderer) override;
};