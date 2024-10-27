#include "engine.hpp"

#include <SDL3/SDL_gpu.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include "ecs.hpp"
#include "texture.hpp"

bool Engine::init()
{
    if (!m_sprite_render_pass.init(
            SDL_GetGPUSwapchainTextureFormat(m_context.device, m_context.window)
        ))
    {
        spdlog::error("Engine::init: failed to initialize sprite render pass");
        return false;
    }

    m_camera = glm::ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT));

    size_t knight_texture_id, block_texture_id;
    try
    {
        knight_texture_id = m_context.texture_registry.add(
            GPUTexture::from_file(m_context.device, "../../assets/knight.png")
        );
        block_texture_id = m_context.texture_registry.add(
            GPUTexture::from_file(m_context.device, "../../assets/block.png")
        );
    }
    catch (...)
    {
        spdlog::error("Engine::init: failed to create textures");
        return false;
    }

    auto knight = m_entities.create();
    m_entities.emplace<Player>(knight);
    m_entities.emplace<Transform>(
        knight,
        glm::vec3(WIDTH / 2.0f, HEIGHT / 2.0f, 0.0f),
        glm::vec3(19.0f, 19.0f, 1.0f)
    );
    m_entities.emplace<Sprite>(knight, knight_texture_id);

    auto block = m_entities.create();
    m_entities.emplace<Transform>(
        block,
        glm::vec3(WIDTH / 2.0f, HEIGHT / 2.0f - 32.0f, 0.0f),
        glm::vec3(19.0f, 19.0f, 1.0f)
    );
    m_entities.emplace<Sprite>(block, block_texture_id);

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

    m_sprite_render_pass.render(cmd_buf, swapchain_texture, m_camera, m_entities);

    SDL_SubmitGPUCommandBuffer(cmd_buf);
}

void Engine::update()
{
    float player_speed = 100.0;
    auto players = m_entities.view<const Player, Transform>();
    for (const auto [entity, transform] : players.each())
    {
        float hori = m_key_states[SDL_SCANCODE_D] - m_key_states[SDL_SCANCODE_A];
        float vert = m_key_states[SDL_SCANCODE_W] - m_key_states[SDL_SCANCODE_S];
        glm::vec3 dir(hori, vert, 0.0f);
        if (glm::length2(dir) > 0)
        {
            dir = glm::normalize(dir);
        }
        transform.position += dir * player_speed * static_cast<float>(m_delta_time);
    }
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
                m_key_states[event.key.scancode] = event.type == SDL_EVENT_KEY_DOWN;
            }
        }

        update();
        render();
    }
    spdlog::trace("Engine::run: exited main loop");
}
