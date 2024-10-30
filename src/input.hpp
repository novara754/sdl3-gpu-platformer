#pragma once

#include <array>

#include <SDL3/SDL.h>

class Input
{
    std::array<bool, SDL_SCANCODE_COUNT> m_key_states{};
    std::array<bool, SDL_SCANCODE_COUNT> m_prev_key_states{};

  public:
    void post_update();

    void handle_event(SDL_KeyboardEvent &event);

    [[nodiscard]] bool is_pressed(SDL_Scancode key) const;
    [[nodiscard]] bool was_just_pressed(SDL_Scancode key) const;
};
