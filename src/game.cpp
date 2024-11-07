#include "game.hpp"

#include <array>

#include <SDL3/SDL_scancode.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/norm.hpp>

#include "ecs.hpp"
#include "engine.hpp"

// clang-format off
static std::array<char[41], 23> map{
    "########################################",
    "#                                      #",
    "#                                      #",
    "#                               C      #",
    "#                              ##      #",
    "#               C                      #",
    "#             #########                #",
    "#                                      #",
    "#                                      #",
    "#                                      #",
    "#       C                 C  C         #",
    "#     #####              ######        #",
    "#                                      #",
    "#                                      #",
    "#               C                      #",
    "#             ######                   #",
    "#                                      #",
    "#     C                  C             #",
    "#    #####              #####          #",
    "#                                      #",
    "#                  P         C         #",
    "#          C                           #",
    "########################################",
};
// clang-format on

bool Game::init()
{
    m_engine->get_systems()->renderer.set_camera(glm::ortho(0.0f, 640.0f, 0.0f, 368.0f));

    auto jump_wav = m_engine->get_systems()->audio.new_source_from_wav("./assets/jump.wav");
    auto pickup_join_wav =
        m_engine->get_systems()->audio.new_source_from_wav("./assets/pickup_coin.wav");

    if (!m_jump_wav || !m_pickup_coin_wav)
    {
        spdlog::error("Game::init: failed to load wav files");
        return false;
    }

    m_jump_wav = *jump_wav;
    m_pickup_coin_wav = *pickup_join_wav;

    size_t knight_texture_id, block_texture_id, bg_texture_id, coin_texture_id;
    try
    {
        knight_texture_id =
            m_engine->get_systems()->renderer.new_texture_from_file("./assets/knight.png");
        block_texture_id =
            m_engine->get_systems()->renderer.new_texture_from_file("./assets/block.png");
        bg_texture_id =
            m_engine->get_systems()->renderer.new_texture_from_file("./assets/background.png");
        coin_texture_id =
            m_engine->get_systems()->renderer.new_texture_from_file("./assets/coin.png");
    }
    catch (std::exception &e)
    {
        spdlog::error("Game::init: failed to create textures: {}", e.what());
        return false;
    }

    m_entities.on_construct<Collider>().connect<&Game::on_add_collider>(this);
    m_entities.on_destroy<Collider>().connect<&Game::on_remove_collider>(this);

    for (size_t row_idx = 0; row_idx < map.size(); ++row_idx)
    {
        const auto &row = map[row_idx];
        for (size_t col_idx = 0; col_idx < 40; ++col_idx)
        {
            auto x = col_idx * 16.0f;
            auto y = VIEWPORT_HEIGHT - (row_idx + 1) * 16.0f;
            const auto cell = row[col_idx];
            switch (cell)
            {
                case '#': {
                    auto block = m_entities.create();
                    m_entities.emplace<Transform>(block, glm::vec2(x, y));
                    m_entities.emplace<Sprite>(block, block_texture_id, glm::ivec2(16, 16));
                    m_entities.emplace<Collider>(
                        block,
                        Collider{
                            .type = Collider::Type::statik,
                            .shape = Collider::Shape::rectangle(glm::vec2(16.0, 16.0)),
                        }
                    );
                    break;
                }
                case 'P': {
                    auto knight = m_entities.create();
                    m_entities.emplace<Player>(knight);
                    m_entities.emplace<Transform>(knight, glm::vec2(x, y));
                    m_entities.emplace<Sprite>(knight, knight_texture_id, glm::ivec2(19, 19));
                    m_entities.emplace<Collider>(
                        knight,
                        Collider{
                            .type = Collider::Type::dynamic,
                            .shape = Collider::Shape::circle(19.0f / 2.0f),
                            .gravity = false,
                        }
                    );
                    break;
                }
                case 'C': {
                    auto coin = m_entities.create();
                    m_entities.emplace<Coin>(coin);
                    m_entities.emplace<Transform>(coin, glm::vec2(x, y));
                    m_entities.emplace<Sprite>(coin, coin_texture_id, glm::ivec2(16, 16));
                    m_entities.emplace<Collider>(
                        coin,
                        Collider{
                            .type = Collider::Type::statik,
                            .shape = Collider::Shape::circle(8.0f),
                            .overlap_only = true,
                        }
                    );
                    break;
                }
                case ' ':
                    break;
                default:
                    spdlog::error("Game::init: invalid cell in map: `{}`", cell);
                    return false;
            }
        }
    }

    auto bg = m_entities.create();
    m_entities.emplace<Transform>(bg, glm::vec2(0.0f));
    m_entities.emplace<Sprite>(
        bg,
        Sprite{
            .texture_id = bg_texture_id,
            .size = glm::ivec2(640, 360),
            .z_index = -10,
        }
    );

    return true;
}

void Game::update(double delta_time)
{
    auto colliders = m_entities.view<Transform, const Collider>();
    for (const auto [entity, transform, collider] : colliders.each())
    {
        transform.position = m_engine->get_systems()->physics.get_position(collider);
    }

    auto players = m_entities.view<const Player, Collider, Sprite>();

    auto coins = m_entities.view<const Coin, const Collider>();
    for (const auto [entity, collider] : coins.each())
    {
        auto other_ids = m_engine->get_systems()->physics.get_contact_others(collider);

        bool hit_player = false;
        for (const auto &other_id : other_ids)
        {
            for (const auto [player_entity, player_collider, player_sprite] : players.each())
            {
                if (other_id == player_collider.id.value())
                {
                    hit_player = true;
                    goto done;
                }
            }
        }

    done:
        if (hit_player)
        {
            m_entities.destroy(entity);
            m_entities.emplace<AudioPlayer>(m_entities.create(), m_pickup_coin_wav);
        }
    }

    float player_speed = 400.0;
    for (const auto [entity, collider, sprite] : players.each())
    {
        float hori = m_engine->get_systems()->input.is_pressed(SDL_SCANCODE_D) -
                     m_engine->get_systems()->input.is_pressed(SDL_SCANCODE_A);
        if (hori > 0.0f)
        {
            sprite.flipped_horizontally = false;
        }
        else if (hori < 0.0f)
        {
            sprite.flipped_horizontally = true;
        }

        glm::vec2 velocity = m_engine->get_systems()->physics.get_velocity(collider);
        velocity.x = hori * player_speed;

        std::optional<glm::vec2> contact_normal =
            m_engine->get_systems()->physics.get_contact_normal(collider);
        bool grounded =
            contact_normal.has_value() && contact_normal->y > 0.0f && contact_normal->x < 0.1;
        if (!grounded)
        {
            velocity.y -= 1000.0f * delta_time;
        }
        else if (m_engine->get_systems()->input.was_just_pressed(SDL_SCANCODE_SPACE))
        {
            m_entities.emplace<AudioPlayer>(m_entities.create(), m_jump_wav);
            velocity.y = 400.0f;
        }

        m_engine->get_systems()->physics.set_velocity(collider, velocity);
    }

    auto audio_players = m_entities.view<const AudioPlayer>();
    for (const auto [entity, player] : audio_players.each())
    {
        m_engine->get_systems()->audio.play(player.source);
        m_entities.destroy(entity);
    }
}

const entt::registry &Game::get_entities() const
{
    return m_entities;
}

void Game::on_add_collider(entt::registry &registry, entt::entity entity)
{
    const auto &transform = registry.get<const Transform>(entity);
    auto &collider = registry.get<Collider>(entity);
    m_engine->get_systems()->physics.add(transform, collider);
}

void Game::on_remove_collider(entt::registry &registry, entt::entity entity)
{
    auto &collider = registry.get<Collider>(entity);
    m_engine->get_systems()->physics.remove(collider);
}
