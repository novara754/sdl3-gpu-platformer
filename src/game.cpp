#include "game.hpp"

#include <SDL3/SDL_scancode.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
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

    size_t knight_texture_id, block_texture_id, bg_texture_id;
    try
    {
        knight_texture_id = m_context->texture_registry.add(
            GPUTexture::from_file(m_context->device, "../../assets/knight.png")
        );
        block_texture_id = m_context->texture_registry.add(
            GPUTexture::from_file(m_context->device, "../../assets/block.png")
        );
        bg_texture_id = m_context->texture_registry.add(
            GPUTexture::from_file(m_context->device, "../../assets/background.png")
        );
    }
    catch (std::exception &e)
    {
        spdlog::error("Game::init: failed to create textures: {}", e.what());
        return false;
    }

    auto bg = m_entities.create();
    m_entities.emplace<Transform>(bg, glm::vec2(0.0f));
    m_entities.emplace<Sprite>(bg, bg_texture_id, glm::ivec2(640, 360));

    auto knight = m_entities.create();
    m_entities.emplace<Player>(knight);
    m_entities.emplace<Transform>(knight, glm::vec2(VIEWPORT_WIDTH / 2.0f, VIEWPORT_HEIGHT / 2.0f));
    m_entities.emplace<Sprite>(knight, knight_texture_id, glm::ivec2(19, 19));
    m_entities.emplace<Collider>(
        knight,
        Collider{
            .type = Collider::Type::dynamic,
            .shape = Collider::Shape::circle(19.0f / 2.0f),
            .gravity = false,
        }
    );

    for (int x = 0; x < VIEWPORT_WIDTH; x += 16)
    {
        auto block = m_entities.create();
        m_entities.emplace<Transform>(block, glm::vec2(x, 0.0f));
        m_entities.emplace<Sprite>(block, block_texture_id, glm::ivec2(16, 16));
        m_entities.emplace<Collider>(
            block,
            Collider{
                .type = Collider::Type::statik,
                .shape = Collider::Shape::rectangle(glm::vec2(16.0, 16.0)),
            }
        );
    }

    for (int x = 32; x < VIEWPORT_WIDTH - 32; x += 16)
    {
        auto block = m_entities.create();
        m_entities.emplace<Transform>(block, glm::vec2(x, 64.0f));
        m_entities.emplace<Sprite>(block, block_texture_id, glm::ivec2(16, 16));
        m_entities.emplace<Collider>(
            block,
            Collider{
                .type = Collider::Type::statik,
                .shape = Collider::Shape::rectangle(glm::vec2(16.0, 16.0)),
            }
        );
    }

    auto colliders = m_entities.view<const Transform, Collider>();
    for (const auto [entity, transform, collider] : colliders.each())
    {
        m_context->physics.add(transform, collider);
    }

    return true;
}

void Game::update([[maybe_unused]] double delta_time)
{
    auto colliders = m_entities.view<Transform, const Collider>();
    for (const auto [entity, transform, collider] : colliders.each())
    {
        transform.position = m_context->physics.get_position(collider);
    }

    float player_speed = 400.0;
    auto players = m_entities.view<const Player, Collider, Sprite>();
    for (const auto [entity, collider, sprite] : players.each())
    {
        float hori = m_context->key_states[SDL_SCANCODE_D] - m_context->key_states[SDL_SCANCODE_A];
        if (hori > 0.0f)
        {
            sprite.flipped_horizontally = false;
        }
        else if (hori < 0.0f)
        {
            sprite.flipped_horizontally = true;
        }

        glm::vec2 velocity = m_context->physics.get_velocity(collider);
        velocity.x = hori * player_speed;

        std::optional<glm::vec2> contact_normal = m_context->physics.get_contact_normal(collider);
        bool grounded = contact_normal.has_value() && contact_normal->y > 0.0f;
        if (!grounded)
        {
            velocity.y -= 1000.0f * delta_time;
        }
        else if (m_context->key_states[SDL_SCANCODE_SPACE])
        {
            velocity.y = 400.0f;
        }

        m_context->physics.set_velocity(collider, velocity);
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
