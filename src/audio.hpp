#pragma once

#include <string>

#include <SDL3/SDL_audio.h>
#include <spdlog/spdlog.h>

struct AudioSource
{
    uint8_t *data;
    uint32_t data_len;
    SDL_AudioStream *stream;

    void play() const
    {
        SDL_PutAudioStreamData(this->stream, this->data, this->data_len);
    }
};

class Audio
{
    SDL_AudioDeviceID m_device{0};
    SDL_AudioSpec m_device_spec{};

    std::vector<AudioSource> m_audio_sources;

    Audio(const Audio &) = delete;
    Audio &operator=(const Audio &) = delete;
    Audio(Audio &&) = delete;
    Audio &operator=(Audio &&) = delete;

  public:
    Audio() = default;

    ~Audio();

    bool init();

    [[nodiscard]] const AudioSource *new_source_from_wav(const std::string &path);
};
