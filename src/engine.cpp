#include "engine.hpp"

#include <SDL3/SDL_gpu.h>

bool Engine::init()
{
    if (!m_systems.renderer.init(m_window))
    {
        spdlog::error("Engine::init: failed to initialize renderer");
        return false;
    }
    spdlog::info("Engine::init renderer initialized");

    if (!m_systems.audio.init())
    {
        spdlog::error("Engine::init: failed to initialize audio");
        return false;
    }
    spdlog::info("Engine::init audio initialized");

    if (!m_game.init())
    {
        spdlog::error("Engine::init: failed to initialize game");
        return false;
    }
    spdlog::info("Engine::init game initialized");

    return true;
}

void Engine::render()
{
    m_systems.renderer.render(m_game.get_entities());
}

void Engine::update()
{
    m_systems.physics.update(m_delta_time);
    m_game.update(m_delta_time);

    m_systems.input.post_update();
}

void Engine::run()
{
    m_last_frame_time = SDL_GetTicks() / 1000.0;

    spdlog::trace("Engine::run: entering main loop");
    while (true)
    {
        double now = SDL_GetTicks() / 1000.0;
        m_delta_time = now - m_last_frame_time;
        m_last_frame_time = now;

        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                return;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
            {
                m_systems.input.handle_event(event.key);
            }
        }

        update();
        render();
    }
    spdlog::trace("Engine::run: exited main loop");
}
