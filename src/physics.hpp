#pragma once

#include <optional>

#include <box2d/box2d.h>
#include <glm/glm.hpp>

typedef b2BodyId PhysicsBodyId;

struct Transform;
struct Collider;

class Physics
{
    b2WorldId m_world_id;

    Physics(const Physics &) = delete;
    Physics &operator=(const Physics &) = delete;
    Physics(Physics &&) = delete;
    Physics &operator=(Physics &&) = delete;

  public:
    Physics();
    ~Physics();

    void add(const Transform &transform, Collider &collider);

    void update(double delta_time);

    [[nodiscard]] glm::vec2 get_position(const Collider &collider) const;

    void set_velocity(const Collider &collider, const glm::vec2 &velocity);
    [[nodiscard]] glm::vec2 get_velocity(const Collider &collider) const;

    [[nodiscard]] std::optional<glm::vec2> get_contact_normal(const Collider &collider) const;
};
