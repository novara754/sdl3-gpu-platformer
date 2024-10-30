#include "input.hpp"

#include <algorithm>

void Input::post_update()
{
    std::copy(std::begin(m_key_states), std::end(m_key_states), std::begin(m_prev_key_states));
}

void Input::handle_event(SDL_KeyboardEvent &event)
{
    m_key_states[event.scancode] = event.type == SDL_EVENT_KEY_DOWN;
}

[[nodiscard]] bool Input::is_pressed(SDL_Scancode key) const
{
    return m_key_states[key];
}

[[nodiscard]] bool Input::was_just_pressed(SDL_Scancode key) const
{
    return !m_prev_key_states[key] && m_key_states[key];
}
