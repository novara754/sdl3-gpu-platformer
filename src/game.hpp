#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "context.hpp"

class Game
{
  public:
    static constexpr int VIEWPORT_WIDTH = 640;
    static constexpr int VIEWPORT_HEIGHT = 360;

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
