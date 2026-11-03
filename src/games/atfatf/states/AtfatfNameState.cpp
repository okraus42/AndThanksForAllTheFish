#include "AtfatfMenuState.hpp"
#include "AtfatfGameState.hpp"
#include "AtfatfNameState.hpp"
#include "engine/Engine.hpp"
#include "logger.hpp"

#include <algorithm>
#include <iterator>

AtfatfNameState::AtfatfNameState(Engine& eng) : engine(eng) {
   
}

void AtfatfNameState::moveSelection(int direction)
{
     (void)direction;
}

void AtfatfNameState::handleInput(InputKey input) {
    switch (input) {
        case InputKey::ENTER:
			if (name.size() < 3)
				break ;
            engine.getAudio().stopMusic();
            engine.setState(std::make_unique<AtfatfGameState>(engine, name, coalitionID, campus));
            break;

        case InputKey::QUIT:
            engine.stop();
            break;

        case InputKey::LEFT:
            if (coalitionID > 0)
                --coalitionID;
            break;

        case InputKey::RIGHT:
            if (coalitions.size() > coalitionID + 1UL)
                ++coalitionID;
            break;

        case InputKey::ESCAPE:
            engine.setState(std::make_unique<AtfatfMenuState>(engine));
            break;

        case InputKey::BACKSPACE:
            if (!name.empty())
                name.pop_back();
            break;

        case InputKey::A: name += 'a'; break;
        case InputKey::B: name += 'b'; break;
        case InputKey::C: name += 'c'; break;
        case InputKey::D: name += 'd'; break;
        case InputKey::E: name += 'e'; break;
        case InputKey::F: name += 'f'; break;
        case InputKey::G: name += 'g'; break;
        case InputKey::H: name += 'h'; break;
        case InputKey::I: name += 'i'; break;
        case InputKey::J: name += 'j'; break;
        case InputKey::K: name += 'k'; break;
        case InputKey::L: name += 'l'; break;
        case InputKey::M: name += 'm'; break;
        case InputKey::N: name += 'n'; break;
        case InputKey::O: name += 'o'; break;
        case InputKey::P: name += 'p'; break;
        case InputKey::Q: name += 'q'; break;
        case InputKey::R: name += 'r'; break;
        case InputKey::S: name += 's'; break;
        case InputKey::T: name += 't'; break;
        case InputKey::U: name += 'u'; break;
        case InputKey::V: name += 'v'; break;
        case InputKey::W: name += 'w'; break;
        case InputKey::X: name += 'x'; break;
        case InputKey::Y: name += 'y'; break;
        case InputKey::Z: name += 'z'; break;

        case InputKey::NUM_0: name += '0'; break;
        case InputKey::NUM_1: name += '1'; break;
        case InputKey::NUM_2: name += '2'; break;
        case InputKey::NUM_3: name += '3'; break;
        case InputKey::NUM_4: name += '4'; break;
        case InputKey::NUM_5: name += '5'; break;
        case InputKey::NUM_6: name += '6'; break;
        case InputKey::NUM_7: name += '7'; break;
        case InputKey::NUM_8: name += '8'; break;
        case InputKey::NUM_9: name += '9'; break;

        case InputKey::LEFT_SHIFT:
            name += '_';
            break;

        case InputKey::MINUS:
            name += '-';
            break;

        default:
            break;
    }
	 if (name.size() > 8)
		name.pop_back();
}

void AtfatfNameState::update(double) {}

void AtfatfNameState::render(IRenderer& renderer) {
	LOG_TRACE("in name loop");
    renderer.drawText(" And Thanks For All The Fish ", 1, 1);
	renderer.drawText(" Enter name: ", 2, 2);
	renderer.drawText(name, 3, 3);
	renderer.drawText(" Choose coalition ", 2, 4);
	renderer.drawText(" < " + coalitions[coalitionID] + " > ", 3, 5);
	// renderer.drawText(" New Game ", 4, 2);
	// if (activeButton == ButtonSelected::Tournament)
	// 	renderer.drawText(" Tournament ", 2, 3);
	// else
	// 	renderer.drawText(" Tournament ", 3, 3);
	// if (activeButton == ButtonSelected::Highscore)
	// 	renderer.drawText(" Highscore ", 2, 4);
	// else
	// 	renderer.drawText(" Highscore ", 3, 4);
	// renderer.drawText(" Settings ", 4, 5);
	// if (activeButton == ButtonSelected::Exit)
	// 	renderer.drawText(" Exit ", 2, 6);
	// else
	// 	renderer.drawText(" Exit ", 3, 6);
	
	renderer.drawText("Press [Enter] to continue.", 4, 7);
	// draw button
}