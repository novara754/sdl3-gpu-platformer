#include <array>

#include <SDL3/SDL.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#include "engine.hpp"

int main()
{
    std::array<spdlog::sink_ptr, 2> sinks{
        std::make_shared<spdlog::sinks::stdout_sink_st>(),
        std::make_shared<spdlog::sinks::basic_file_sink_st>("platformer-log.txt"),
    };
    auto combined_logger =
        std::make_shared<spdlog::logger>("name", std::begin(sinks), std::end(sinks));
    spdlog::register_logger(combined_logger);
    spdlog::set_default_logger(combined_logger);
    spdlog::set_level(spdlog::level::trace);

    SDL_SetAppMetadata("Platformer", "0.1", nullptr);
    spdlog::trace("main: set sdl app metadata");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        spdlog::error("main: failed to initialize sdl: {}", SDL_GetError());
        return 1;
    }
    spdlog::trace("main: initialized sdl video and audio subsystem");

    SDL_Window *window = SDL_CreateWindow("Platformer", WIDTH, HEIGHT, 0);
    if (!window)
    {
        spdlog::error("main: failed to create window and renderer: {}", SDL_GetError());
        return 1;
    }
    spdlog::trace("main: created sdl window");

    {
        Engine engine(window);
        if (engine.init())
        {
            engine.run();
        }
        else
        {
            spdlog::error("main: failed to initialize engine");
        }
    }

    SDL_DestroyWindow(window);

    spdlog::trace("main: process terminating...");
    return 0;
}
