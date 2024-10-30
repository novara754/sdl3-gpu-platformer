#pragma once

#include <SDL3/SDL_gpu.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include "texture.hpp"

struct GPUContext;

class SpriteRenderPass
{
    struct Uniforms
    {
        glm::mat4 camera;
        glm::mat4 model;
        glm::vec2 flipped;
    };

    GPUContext *m_gpu_context;
    GPUTexture m_depth_texture;
    SDL_GPUGraphicsPipeline *m_pipeline{nullptr};

    SpriteRenderPass(const SpriteRenderPass &) = delete;
    SpriteRenderPass &operator=(const SpriteRenderPass &) = delete;
    SpriteRenderPass(SpriteRenderPass &&) = delete;
    SpriteRenderPass &operator=(SpriteRenderPass &&) = delete;

  public:
    SpriteRenderPass(GPUContext *gpu_context) : m_gpu_context(gpu_context)
    {
    }

    [[nodiscard]] bool init(
        SDL_GPUTextureFormat swapchain_texture_format, uint32_t surface_width,
        uint32_t surface_height
    );

    void release();

    void render(
        SDL_GPUCommandBuffer *cmd_buffer, SDL_GPUTexture *target_texture, const glm::mat4 &camera,
        const entt::registry &entities
    );
};
