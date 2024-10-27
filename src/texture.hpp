#pragma once

#include <string>
#include <utility>

#include <SDL3/SDL_gpu.h>

class GPUTexture
{
    SDL_GPUDevice *m_device{nullptr};
    SDL_GPUTexture *m_texture{nullptr};
    SDL_GPUSampler *m_sampler{nullptr};
    GPUTexture(const GPUTexture &) = delete;
    GPUTexture &operator=(const GPUTexture &) = delete;

    explicit GPUTexture(SDL_GPUDevice *device, SDL_GPUTexture *texture, SDL_GPUSampler *sampler)
        : m_device{device}, m_texture{texture}, m_sampler{sampler}
    {
    }

  public:
    [[nodiscard]] static GPUTexture from_file(SDL_GPUDevice *device, const std::string &path);

    GPUTexture() = default;

    GPUTexture(GPUTexture &&other)
        : m_device{std::exchange(other.m_device, nullptr)},
          m_texture{std::exchange(other.m_texture, nullptr)},
          m_sampler{std::exchange(other.m_sampler, nullptr)}
    {
    }

    GPUTexture &operator=(GPUTexture &&other)
    {
        std::swap(m_device, other.m_device);
        std::swap(m_texture, other.m_texture);
        std::swap(m_sampler, other.m_sampler);
        return *this;
    }

    [[nodiscard]] SDL_GPUTextureSamplerBinding get_binding() const noexcept;

    ~GPUTexture()
    {
        if (m_device)
        {
            SDL_ReleaseGPUSampler(m_device, m_sampler);
            SDL_ReleaseGPUTexture(m_device, m_texture);
        }
    }
};
