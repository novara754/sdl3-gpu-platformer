#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "audio.hpp"

class Engine;

class Game
{
  public:
    static constexpr int VIEWPORT_WIDTH = 640;
    static constexpr int VIEWPORT_HEIGHT = 368;

  private:
    Engine *m_engine;
    entt::registry m_entities;

    AudioSourceId m_jump_wav;
    AudioSourceId m_pickup_coin_wav;

  public:
    Game(Engine *engine) : m_engine(engine)
    {
    }

    bool init();

    void update(double delta_time);

    const entt::registry &get_entities() const;

  private:
    void on_add_collider(entt::registry &registry, entt::entity entity);
    void on_remove_collider(entt::registry &registry, entt::entity entity);
};
