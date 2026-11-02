#include "TerminalInput.hpp"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

static struct termios original;

TerminalInput::TerminalInput() {
    enableRawMode();
}

TerminalInput::~TerminalInput() {
    disableRawMode();
}

void TerminalInput::enableRawMode() {
    tcgetattr(STDIN_FILENO, &original);

    struct termios raw = original;
    raw.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void TerminalInput::disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
}

InputKey TerminalInput::pollInput() {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1)
        return InputKey::NONE;

    switch (c) {
        case 'w': return InputKey::UP;
        case 's': return InputKey::DOWN;
        case 'a': return InputKey::LEFT;
        case 'd': return InputKey::RIGHT;
        case '\n': return InputKey::ENTER;
        case 27: return InputKey::ESCAPE;
        case 'q': return InputKey::Q;
    }
    return InputKey::NONE;
}