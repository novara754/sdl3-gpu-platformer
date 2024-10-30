#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include <entt/entt.hpp>

#include "registry.hpp"
#include "sprite_render_pass.hpp"
#include "texture.hpp"

typedef size_t TextureId;

struct GPUContext
{
    SDL_GPUDevice *device;
    Registry<GPUTexture> textures;
};

class Renderer
{
    SDL_Window *m_window{nullptr};
    GPUContext m_gpu_context;

    glm::mat4 m_camera;
    SpriteRenderPass m_sprite_render_pass;

  public:
    Renderer() : m_sprite_render_pass(&m_gpu_context)
    {
    }

    ~Renderer()
    {
        if (m_gpu_context.device != nullptr)
        {
            m_sprite_render_pass.release();

            m_gpu_context.textures.for_each([&](GPUTexture &texture) {
                texture.release(m_gpu_context.device);
            });

            SDL_DestroyGPUDevice(m_gpu_context.device);
        }
    }

    [[nodiscard]] bool init(SDL_Window *window);

    void render(const entt::registry &entities);

    void set_camera(const glm::mat4 &camera)
    {
        m_camera = camera;
    }

    [[nodiscard]] TextureId new_texture_from_file(const std::string &path);
};
