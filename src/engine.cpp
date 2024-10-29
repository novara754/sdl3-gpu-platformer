#include "engine.hpp"

#include <SDL3/SDL_gpu.h>

bool Engine::init()
{
    if (!m_sprite_render_pass.init(
            SDL_GetGPUSwapchainTextureFormat(m_context.device, m_context.window)
        ))
    {
        spdlog::error("Engine::init: failed to initialize sprite render pass");
        return false;
    }

    if (!m_game.init())
    {
        spdlog::error("Engine::init: failed to initialize game");
        return false;
    }

    return true;
}

void Engine::render()
{
    SDL_GPUCommandBuffer *cmd_buf = SDL_AcquireGPUCommandBuffer(m_context.device);
    if (!cmd_buf)
    {
        spdlog::error("Engine::render: failed to acquire gpu command buffer: {}", SDL_GetError());
        return;
    }

    SDL_GPUTexture *swapchain_texture;
    if (!SDL_AcquireGPUSwapchainTexture(
            cmd_buf,
            m_context.window,
            &swapchain_texture,
            nullptr,
            nullptr
        ))
    {
        spdlog::error(
            "Engine::render: failed to acquire gpu swapchain texture: {}",
            SDL_GetError()
        );
        return;
    }

    m_sprite_render_pass
        .render(cmd_buf, swapchain_texture, m_game.get_camera(), m_game.get_entities());

    SDL_SubmitGPUCommandBuffer(cmd_buf);
}

void Engine::update()
{
    m_context.physics.update(m_delta_time);
    m_game.update(m_delta_time);
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
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                return;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
            {
                m_context.key_states[event.key.scancode] = event.type == SDL_EVENT_KEY_DOWN;
            }
        }

        update();
        render();
    }
    spdlog::trace("Engine::run: exited main loop");
}
