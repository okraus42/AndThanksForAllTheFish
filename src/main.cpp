#include "engine/Engine.hpp"
#include "render/TerminalRenderer.hpp"
#include "render/SDLRenderer.hpp"
#include "audio/SDLAudio.hpp"

#include "games/atfatf/states/AtfatfMenuState.hpp"

#include "logger.hpp"

void	initLogger() {
	Logger::Config config;
    config.level = Logger::Level::Info;
    config.timestamp = Logger::TimestampPrecision::Milliseconds;
    config.show_source = true;
    config.colors = true;

    Logger::instance().init(config);
}

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	initLogger();
	LOG_DEBUG("Logger initialised");
    SDLRenderer renderer;
    SDLAudio audio;
    if (!renderer.init())
        return 1;
    Engine engine(renderer, audio);

    engine.setState(std::make_unique<AtfatfMenuState>(engine));
    engine.run();

    renderer.shutdown();
    return 0;
}
