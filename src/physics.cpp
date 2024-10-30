#include "physics.hpp"

#include <box2d/box2d.h>
#include <box2d/math_functions.h>

#include "ecs.hpp"

Physics::Physics()
{
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity.y = -10.0f;
    world_def.maximumLinearVelocity = 1'000'000.0f;
    world_def.restitutionThreshold = world_def.maximumLinearVelocity;
    world_def.contactPushoutVelocity = 1'000.0f;
    m_world_id = b2CreateWorld(&world_def);
}

Physics::~Physics()
{
    b2DestroyWorld(m_world_id);
}

void Physics::add(const Transform &transform, Collider &collider)
{
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.position = b2Vec2{transform.position.x, transform.position.y};
    body_def.linearVelocity = b2Vec2{collider.velocity.x, collider.velocity.y};
    body_def.type = [&]() {
        switch (collider.type)
        {
            case Collider::Type::statik:
                return b2_staticBody;
            case Collider::Type::dynamic:
                return b2_dynamicBody;
        }
    }();
    body_def.gravityScale = collider.gravity ? 1.0f : 0.0f;
    b2BodyId body_id = b2CreateBody(m_world_id, &body_def);

    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = 1'000.0f;
    switch (collider.shape.type)
    {
        case Collider::Shape::Type::rectangle: {
            b2Polygon box = b2MakeBox(collider.shape.size.x / 2.0f, collider.shape.size.y / 2.0f);
            b2CreatePolygonShape(body_id, &shape_def, &box);
            break;
        }
        case Collider::Shape::Type::circle: {
            b2Circle circle{
                .center = b2Vec2{0.0f, 0.0f},
                .radius = collider.shape.radius,
            };
            b2CreateCircleShape(body_id, &shape_def, &circle);
            break;
        }
    }

    collider.id = body_id;
}

void Physics::update(double delta_time)
{
    b2World_Step(m_world_id, static_cast<float>(delta_time), 4);
}

glm::vec2 Physics::get_position(const Collider &collider) const
{
    b2Vec2 position = b2Body_GetPosition(collider.id.value());
    return glm::vec2(position.x, position.y);
}

void Physics::set_velocity(const Collider &collider, const glm::vec2 &velocity)
{
    b2Body_SetLinearVelocity(collider.id.value(), b2Vec2{velocity.x, velocity.y});
}

glm::vec2 Physics::get_velocity(const Collider &collider) const
{
    b2Vec2 velocity = b2Body_GetLinearVelocity(collider.id.value());
    return glm::vec2(velocity.x, velocity.y);
}

[[nodiscard]] std::optional<glm::vec2> Physics::get_contact_normal(const Collider &collider) const
{
    b2ContactData data;
    if (b2Body_GetContactData(collider.id.value(), &data, 1) > 0)
    {
        return glm::vec2(data.manifold.normal.x, data.manifold.normal.y);
    }
    else
    {
        return {};
    }
}
