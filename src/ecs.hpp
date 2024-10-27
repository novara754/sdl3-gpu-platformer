#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

struct Transform
{
    glm::vec3 position;
    glm::vec3 scale;

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
};

struct Player
{
};
