#include "audio.hpp"

Audio::~Audio()
{
    if (m_device != 0)
    {
        SDL_CloseAudioDevice(m_device);
    }

    for (const auto &source : m_audio_sources)
    {
        SDL_free(source.data);
        SDL_DestroyAudioStream(source.stream);
    }
}

bool Audio::init()
{
    m_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (m_device == 0)
    {
        spdlog::error("Audio::init: failed to open audio device: {}", SDL_GetError());
        return false;
    }

    if (!SDL_GetAudioDeviceFormat(m_device, &m_device_spec, nullptr))
    {
        spdlog::error("Audio::init: failed to get audio device format: {}", SDL_GetError());
        return false;
    }

    return true;
}

[[nodiscard]] std::optional<AudioSourceId> Audio::new_source_from_wav(const std::string &path)
{
    SDL_AudioSpec spec;
    uint8_t *data;
    uint32_t len;
    if (!SDL_LoadWAV(path.c_str(), &spec, &data, &len))
    {
        spdlog::error("Audio::new_source_from_wav: failed to open wav: {}", SDL_GetError());
        return {};
    }

    SDL_AudioStream *stream = SDL_CreateAudioStream(&spec, &m_device_spec);
    if (stream == nullptr)
    {
        SDL_free(data);
        spdlog::error(
            "Audio::new_source_from_wav: failed to create audio stream: {}",
            SDL_GetError()
        );
        return {};
    }

    if (!SDL_BindAudioStream(m_device, stream))
    {
        SDL_free(data);
        SDL_DestroyAudioStream(stream);
        spdlog::error(
            "Audio::new_source_from_wav: failed to bind audio stream: {}",
            SDL_GetError()
        );
        return {};
    }

    m_audio_sources.emplace_back(data, len, stream);
    return m_audio_sources.size() - 1;
}

void Audio::play(AudioSourceId id) const
{
    const auto &source = m_audio_sources[id];
    if (!SDL_PutAudioStreamData(source.stream, source.data, source.data_len))
    {
        spdlog::error("AudioSource::play: failed to play audio: {}", SDL_GetError());
    }
}
