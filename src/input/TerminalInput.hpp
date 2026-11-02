#pragma once
#include "InputKey.hpp"

class TerminalInput {
public:
    TerminalInput();
    ~TerminalInput();

    InputKey pollInput();

private:
    void enableRawMode();
    void disableRawMode();
};