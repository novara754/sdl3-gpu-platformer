#pragma once

#include <SDL3/SDL.h>
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

#include "game.hpp"
#include "systems.hpp"

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 736;

class Engine
{
    SDL_Window *m_window;

    double m_last_frame_time{0.0};
    double m_delta_time{0.0};

    Systems m_systems;
    Game m_game;

    Engine() = delete;
    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;
    Engine(Engine &&) = delete;
    Engine &operator=(Engine &&) = delete;

  public:
    Engine(SDL_Window *window) : m_window(window), m_game(this)
    {
    }

    [[nodiscard]] bool init();

    void run();

    [[nodiscard]] Systems *get_systems()
    {
        return &m_systems;
    }

  private:
    void render();
    void update();
};
