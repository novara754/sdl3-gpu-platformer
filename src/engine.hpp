#pragma once

#include <vector>

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "context.hpp"
#include "sprite_render_pass.hpp"

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;

class Engine
{
    Context m_context;

    SpriteRenderPass m_sprite_render_pass;
    glm::mat4 m_camera;
    std::vector<Sprite> m_sprites;

    Engine() = delete;
    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;
    Engine(Engine &&) = delete;
    Engine &operator=(Engine &&) = delete;

  public:
    Engine(SDL_Window *window, SDL_GPUDevice *device)
        : m_context(window, device), m_sprite_render_pass(&m_context)

    {
    }

    [[nodiscard]] bool init();

    void run();
};
