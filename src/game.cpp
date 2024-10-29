#include "game.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include "ecs.hpp"

bool Game::init()
{
    m_camera = glm::ortho(
        0.0f,
        static_cast<float>(VIEWPORT_WIDTH),
        0.0f,
        static_cast<float>(VIEWPORT_HEIGHT)
    );

    size_t knight_texture_id, block_texture_id;
    try
    {
        knight_texture_id = m_context->texture_registry.add(
            GPUTexture::from_file(m_context->device, "../../assets/knight.png")
        );
        block_texture_id = m_context->texture_registry.add(
            GPUTexture::from_file(m_context->device, "../../assets/block.png")
        );
    }
    catch (std::exception &e)
    {
        spdlog::error("Game::init: failed to create textures: {}", e.what());
        return false;
    }

    auto knight = m_entities.create();
    m_entities.emplace<Player>(knight);
    m_entities.emplace<Transform>(
        knight,
        glm::vec3(VIEWPORT_WIDTH / 2.0f, VIEWPORT_HEIGHT / 2.0f, 0.0f)
    );
    m_entities.emplace<Sprite>(knight, knight_texture_id, glm::ivec2(19, 19));

    for (int i = 0; i < 10; ++i)
    {
        auto block = m_entities.create();
        m_entities.emplace<Transform>(
            block,
            glm::vec3(VIEWPORT_WIDTH / 2.0f + (i - 5.0f) * 16.0f, VIEWPORT_HEIGHT / 2.0f, 0.0f)
        );
        m_entities.emplace<Sprite>(block, block_texture_id, glm::ivec2(16, 16));
    }

    return true;
}

void Game::update(double delta_time)
{
    float player_speed = 100.0;
    auto players = m_entities.view<const Player, Transform, Sprite>();
    for (const auto [entity, transform, sprite] : players.each())
    {
        float hori = m_context->key_states[SDL_SCANCODE_D] - m_context->key_states[SDL_SCANCODE_A];
        float vert = m_context->key_states[SDL_SCANCODE_W] - m_context->key_states[SDL_SCANCODE_S];
        glm::vec3 dir(hori, vert, 0.0f);
        if (glm::length2(dir) > 0.0f)
        {
            dir = glm::normalize(dir);
        }
        transform.position += dir * player_speed * static_cast<float>(delta_time);
        if (dir.x > 0.0f)
        {
            sprite.flipped_horizontally = false;
        }
        else if (dir.x < 0.0f)
        {
            sprite.flipped_horizontally = true;
        }
    }
}

const glm::mat4 &Game::get_camera() const
{
    return m_camera;
}

const entt::registry &Game::get_entities() const
{
    return m_entities;
}
