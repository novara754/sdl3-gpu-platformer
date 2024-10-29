#pragma once

#include <string>
#include <utility>

#include <SDL3/SDL_gpu.h>
#include <spdlog/spdlog.h>

class GPUTexture
{
    std::string m_name;
    SDL_GPUDevice *m_device{nullptr};
    SDL_GPUTexture *m_texture{nullptr};
    SDL_GPUSampler *m_sampler{nullptr};

    GPUTexture(const GPUTexture &) = delete;
    GPUTexture &operator=(const GPUTexture &) = delete;

    explicit GPUTexture(
        SDL_GPUDevice *device, SDL_GPUTexture *texture, SDL_GPUSampler *sampler, std::string name
    )
        : m_name(name), m_device{device}, m_texture{texture}, m_sampler{sampler}
    {
    }

  public:
    [[nodiscard]] static GPUTexture from_file(SDL_GPUDevice *device, const std::string &path);

    [[nodiscard]] static GPUTexture
    depth_target(SDL_GPUDevice *device, uint32_t width, uint32_t height);

    GPUTexture() = default;

    GPUTexture(GPUTexture &&other)
        : m_name(std::move(other.m_name)), m_device(std::exchange(other.m_device, nullptr)),
          m_texture(std::exchange(other.m_texture, nullptr)),
          m_sampler(std::exchange(other.m_sampler, nullptr))
    {
    }

    GPUTexture &operator=(GPUTexture &&other)
    {
        m_name = other.m_name;
        std::swap(m_device, other.m_device);
        std::swap(m_texture, other.m_texture);
        std::swap(m_sampler, other.m_sampler);
        return *this;
    }

    [[nodiscard]] SDL_GPUTexture *get_texture()
    {
        return m_texture;
    }

    [[nodiscard]] SDL_GPUTextureSamplerBinding get_binding() const noexcept
    {
        return SDL_GPUTextureSamplerBinding{
            .texture = m_texture,
            .sampler = m_sampler,
        };
    }

    ~GPUTexture()
    {
        if (m_device)
        {
            SDL_ReleaseGPUSampler(m_device, m_sampler);
            SDL_ReleaseGPUTexture(m_device, m_texture);
            spdlog::trace("GPUTexture::~GPUTexture: released sampler and texture for `{}`", m_name);
        }
    }
};
