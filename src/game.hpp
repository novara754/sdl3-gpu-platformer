#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "context.hpp"

class Game
{
  public:
    static constexpr int RENDER_WIDTH = 1280;
    static constexpr int RENDER_HEIGHT = 720;

  private:
    Context *m_context;
    glm::mat4 m_camera;
    entt::registry m_entities;

  public:
    Game(Context *context) : m_context(context)
    {
    }

    bool init();

    void update(double delta_time);

    const glm::mat4 &get_camera() const;

    const entt::registry &get_entities() const;
};
