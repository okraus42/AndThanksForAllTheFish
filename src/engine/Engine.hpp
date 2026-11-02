#pragma once

#include <memory>
#include "IGameState.hpp"
#include "audio/IAudio.hpp"
#include "render/IRenderer.hpp"

class Engine {
public:
    Engine(IRenderer& renderer, IAudio& audio);

    void setState(std::unique_ptr<IGameState> newState);
    void run();
    void stop();

    IRenderer& getRenderer();
    IAudio& getAudio();

private:
    std::unique_ptr<IGameState> currentState;
    IRenderer& renderer;
    IAudio& audio;

    bool running = true;
};