#pragma once

#include <SDL3/SDL_gpu.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include "context.hpp"

class SpriteRenderPass
{
    struct Uniforms
    {
        glm::mat4 camera;
        glm::mat4 model;
        glm::vec2 flipped;
    };

    Context *m_context;
    SDL_GPUGraphicsPipeline *m_pipeline{nullptr};

    SpriteRenderPass() = delete;
    SpriteRenderPass(const SpriteRenderPass &) = delete;
    SpriteRenderPass &operator=(const SpriteRenderPass &) = delete;
    SpriteRenderPass(SpriteRenderPass &&) = delete;
    SpriteRenderPass &operator=(SpriteRenderPass &&) = delete;

  public:
    SpriteRenderPass(Context *context) : m_context(context)
    {
    }

    ~SpriteRenderPass()
    {
        if (m_pipeline)
        {
            SDL_ReleaseGPUGraphicsPipeline(m_context->device, m_pipeline);
            spdlog::trace("SpriteRenderPass::~SpriteRenderPass: released sprite render pipeline");
        }
    }

    [[nodiscard]] bool init(SDL_GPUTextureFormat swapchain_texture_format);

    void render(
        SDL_GPUCommandBuffer *cmd_buffer, SDL_GPUTexture *target_texture, const glm::mat4 &camera,
        const entt::registry &entities
    ) const;
};
