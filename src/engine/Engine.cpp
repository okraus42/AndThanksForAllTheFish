#include <thread>
#include <chrono>
#include "Engine.hpp"
#include "input/TerminalInput.hpp"
#include "input/SDLInput.hpp"


Engine::Engine(IRenderer& renderer, IAudio& audio)
    : renderer(renderer), audio(audio) {}

void Engine::setState(std::unique_ptr<IGameState> newState) {
    currentState = std::move(newState);
}

IRenderer& Engine::getRenderer() {
    return renderer;
}

IAudio& Engine::getAudio() {
    return audio;
}

void Engine::run() {
    SDLInput input;

    while (running) {
        InputKey key = input.pollInput();

        if (currentState) {
            currentState->handleInput(key);
            currentState->update(0.016);

            renderer.clear();
            currentState->render(renderer);
            renderer.present();
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
    }
}

void Engine::stop() {
    running = false;
}