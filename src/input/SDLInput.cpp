#include "SDLInput.hpp"
#include <SDL3/SDL.h>


static InputKey convertKey(SDL_Keycode key) {
    switch (key) {
        // Letters
        case SDLK_A: return InputKey::A;
        case SDLK_B: return InputKey::B;
        case SDLK_C: return InputKey::C;
        case SDLK_D: return InputKey::D;
        case SDLK_E: return InputKey::E;
        case SDLK_F: return InputKey::F;
        case SDLK_G: return InputKey::G;
        case SDLK_H: return InputKey::H;
        case SDLK_I: return InputKey::I;
        case SDLK_J: return InputKey::J;
        case SDLK_K: return InputKey::K;
        case SDLK_L: return InputKey::L;
        case SDLK_M: return InputKey::M;
        case SDLK_N: return InputKey::N;
        case SDLK_O: return InputKey::O;
        case SDLK_P: return InputKey::P;
        case SDLK_Q: return InputKey::Q;
        case SDLK_R: return InputKey::R;
        case SDLK_S: return InputKey::S;
        case SDLK_T: return InputKey::T;
        case SDLK_U: return InputKey::U;
        case SDLK_V: return InputKey::V;
        case SDLK_W: return InputKey::W;
        case SDLK_X: return InputKey::X;
        case SDLK_Y: return InputKey::Y;
        case SDLK_Z: return InputKey::Z;

        // Number row
        case SDLK_0: return InputKey::NUM_0;
        case SDLK_1: return InputKey::NUM_1;
        case SDLK_2: return InputKey::NUM_2;
        case SDLK_3: return InputKey::NUM_3;
        case SDLK_4: return InputKey::NUM_4;
        case SDLK_5: return InputKey::NUM_5;
        case SDLK_6: return InputKey::NUM_6;
        case SDLK_7: return InputKey::NUM_7;
        case SDLK_8: return InputKey::NUM_8;
        case SDLK_9: return InputKey::NUM_9;

        // Punctuation
        case SDLK_MINUS:        return InputKey::MINUS;
        case SDLK_EQUALS:       return InputKey::EQUALS;
        case SDLK_LEFTBRACKET:  return InputKey::LEFT_BRACKET;
        case SDLK_RIGHTBRACKET: return InputKey::RIGHT_BRACKET;
        case SDLK_BACKSLASH:    return InputKey::BACKSLASH;
        case SDLK_SEMICOLON:    return InputKey::SEMICOLON;
        case SDLK_APOSTROPHE:   return InputKey::APOSTROPHE;
        case SDLK_GRAVE:        return InputKey::GRAVE;
        case SDLK_COMMA:        return InputKey::COMMA;
        case SDLK_PERIOD:       return InputKey::PERIOD;
        case SDLK_SLASH:        return InputKey::SLASH;

        // Whitespace and editing
        case SDLK_SPACE:     return InputKey::SPACE;
        case SDLK_TAB:       return InputKey::TAB;
        case SDLK_RETURN:    return InputKey::ENTER;
        case SDLK_ESCAPE:    return InputKey::ESCAPE;
        case SDLK_BACKSPACE: return InputKey::BACKSPACE;
        case SDLK_DELETE:    return InputKey::DELETE_KEY;
        case SDLK_INSERT:    return InputKey::INSERT;

        // Navigation
        case SDLK_UP:       return InputKey::UP;
        case SDLK_DOWN:     return InputKey::DOWN;
        case SDLK_LEFT:     return InputKey::LEFT;
        case SDLK_RIGHT:    return InputKey::RIGHT;
        case SDLK_HOME:     return InputKey::HOME;
        case SDLK_END:      return InputKey::END;
        case SDLK_PAGEUP:   return InputKey::PAGE_UP;
        case SDLK_PAGEDOWN: return InputKey::PAGE_DOWN;

        // Function keys
        case SDLK_F1:  return InputKey::F1;
        case SDLK_F2:  return InputKey::F2;
        case SDLK_F3:  return InputKey::F3;
        case SDLK_F4:  return InputKey::F4;
        case SDLK_F5:  return InputKey::F5;
        case SDLK_F6:  return InputKey::F6;
        case SDLK_F7:  return InputKey::F7;
        case SDLK_F8:  return InputKey::F8;
        case SDLK_F9:  return InputKey::F9;
        case SDLK_F10: return InputKey::F10;
        case SDLK_F11: return InputKey::F11;
        case SDLK_F12: return InputKey::F12;

        // Modifiers
        case SDLK_LSHIFT:     return InputKey::LEFT_SHIFT;
        case SDLK_RSHIFT:     return InputKey::RIGHT_SHIFT;
        case SDLK_LCTRL:      return InputKey::LEFT_CTRL;
        case SDLK_RCTRL:      return InputKey::RIGHT_CTRL;
        case SDLK_LALT:       return InputKey::LEFT_ALT;
        case SDLK_RALT:       return InputKey::RIGHT_ALT;
        case SDLK_LGUI:       return InputKey::LEFT_GUI;
        case SDLK_RGUI:       return InputKey::RIGHT_GUI;
        case SDLK_CAPSLOCK:   return InputKey::CAPS_LOCK;
        case SDLK_NUMLOCKCLEAR: return InputKey::NUM_LOCK;
        case SDLK_SCROLLLOCK: return InputKey::SCROLL_LOCK;

        // Numeric keypad
        case SDLK_KP_0: return InputKey::KP_0;
        case SDLK_KP_1: return InputKey::KP_1;
        case SDLK_KP_2: return InputKey::KP_2;
        case SDLK_KP_3: return InputKey::KP_3;
        case SDLK_KP_4: return InputKey::KP_4;
        case SDLK_KP_5: return InputKey::KP_5;
        case SDLK_KP_6: return InputKey::KP_6;
        case SDLK_KP_7: return InputKey::KP_7;
        case SDLK_KP_8: return InputKey::KP_8;
        case SDLK_KP_9: return InputKey::KP_9;

        case SDLK_KP_DECIMAL:  return InputKey::KP_DECIMAL;
        case SDLK_KP_DIVIDE:   return InputKey::KP_DIVIDE;
        case SDLK_KP_MULTIPLY: return InputKey::KP_MULTIPLY;
        case SDLK_KP_MINUS:    return InputKey::KP_MINUS;
        case SDLK_KP_PLUS:     return InputKey::KP_PLUS;
        case SDLK_KP_ENTER:   return InputKey::KP_ENTER;
        case SDLK_KP_EQUALS:  return InputKey::KP_EQUALS;

        // Additional keys
        case SDLK_PRINTSCREEN: return InputKey::PRINT_SCREEN;
        case SDLK_PAUSE:       return InputKey::PAUSE;
        case SDLK_MENU:        return InputKey::MENU;

        default:
            return InputKey::NONE;
    }
}

InputKey SDLInput::pollInput() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {

		if (event.type == SDL_EVENT_QUIT) {
			return InputKey::QUIT;
		}

		if (event.type == SDL_EVENT_KEY_DOWN) {
			return convertKey(event.key.key);
		}
	}
    return InputKey::NONE;
}