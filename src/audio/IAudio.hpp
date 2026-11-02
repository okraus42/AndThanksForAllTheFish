#pragma once

#include "games/atfatf/AtfatfEvent.hpp"
#include <string>

class IAudio {
public:
    virtual ~IAudio() = default;

    virtual void playEvent(AtfatfEvent event) = 0;

    virtual void playMusic(const std::string& path, int loops) = 0;
    virtual void stopMusic() = 0;
};
