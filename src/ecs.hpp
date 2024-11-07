#pragma once

#include <optional>

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "audio.hpp"
#include "physics.hpp"
#include "renderer.hpp"

struct Transform
{
    glm::vec2 position{0.0f};
    glm::vec2 scale{1.0f};

    [[nodiscard]] glm::mat4 to_matrix() const
    {
        glm::mat4 mat(1.0f);
        mat = glm::translate(mat, glm::vec3(this->position, 0.0f));
        mat = glm::scale(mat, glm::vec3(this->scale, 1.0f));
        return mat;
    }
};

struct Sprite
{
    TextureId texture_id;
    glm::ivec2 size;
    int z_index{0};
    bool flipped_horizontally{false};
    bool flipped_vertically{false};
};

struct Collider
{
    enum class Type
    {
        statik,
        dynamic
    };

    struct Shape
    {
        enum class Type
        {
            rectangle,
            circle,
        };

        Type type;
        union
        {
            glm::vec2 size;
            float radius;
        };

        static Shape rectangle(const glm::vec2 &size)
        {
            return Shape{
                .type = Type::rectangle,
                .size = size,
            };
        }

        static Shape circle(float radius)
        {
            return Shape{
                .type = Type::circle,
                .radius = radius,
            };
        }
    };

    std::optional<PhysicsBodyId> id{};
    Type type;
    Shape shape;
    glm::vec2 velocity{0.0f};
    bool gravity{true};
    bool overlap_only{false};
};

struct Player
{
};

struct AudioPlayer
{
    const AudioSource *source;
};

struct Coin
{
};
