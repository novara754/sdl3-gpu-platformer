#include "engine.hpp"

#include <SDL3/SDL_gpu.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sprite_render_pass.hpp"
#include "texture.hpp"

bool Engine::init()
{
    if (!m_sprite_render_pass.init(SDL_GetGPUSwapchainTextureFormat(m_device, m_window)))
    {
        spdlog::error("Engine::init: failed to initialize sprite render pass");
        return false;
    }

    m_camera = glm::ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT));

    GPUTexture knight_texture;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(WIDTH / 2.0f, HEIGHT / 2.0f, 0.0f));
    model = glm::scale(model, glm::vec3(64.0f, 64.0f, 1.0f));

    Sprite knight{
        .texture = {},
        .model = model,
    };
    try
    {
        knight.texture = GPUTexture::from_file(m_device, "../../assets/knight.png");
    }
    catch (...)
    {
        spdlog::error("Engine::init: failed to create knight texture");
        return false;
    }

    m_sprites.emplace_back(std::move(knight));

    return true;
}

void Engine::run()
{
    spdlog::trace("Engine::run: entering main loop");
    while (true)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                return;
            }
        }

        SDL_GPUCommandBuffer *cmd_buf = SDL_AcquireGPUCommandBuffer(m_device);
        if (!cmd_buf)
        {
            spdlog::error("Engine::run: failed to acquire gpu command buffer: {}", SDL_GetError());
            return;
        }

        SDL_GPUTexture *swapchain_texture;
        if (!SDL_AcquireGPUSwapchainTexture(
                cmd_buf,
                m_window,
                &swapchain_texture,
                nullptr,
                nullptr
            ))
        {
            spdlog::error(
                "Engine::run: failed to acquire gpu swapchain texture: {}",
                SDL_GetError()
            );
            return;
        }

        m_sprite_render_pass.render(cmd_buf, swapchain_texture, m_camera, m_sprites);

        SDL_SubmitGPUCommandBuffer(cmd_buf);
    }
    spdlog::trace("Engine::run: exited main loop");
}
