#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

struct Transform
{
    glm::vec3 position{0.0f};
    glm::vec3 scale{1.0f};

    [[nodiscard]] glm::mat4 to_matrix() const
    {
        glm::mat4 mat(1.0f);
        mat = glm::translate(mat, this->position);
        mat = glm::scale(mat, this->scale);
        return mat;
    }
};

struct Sprite
{
    size_t texture_id;
    glm::ivec2 size;
    bool flipped_horizontally{false};
    bool flipped_vertically{false};
};

struct Player
{
};
