#pragma once

#include <vector>

template<typename T>
class Registry
{
    std::vector<T> m_storage;

  public:
    [[nodiscard]] size_t add(T &&obj)
    {
        size_t ret = m_storage.size();
        m_storage.emplace_back(std::move(obj));
        return ret;
    }

    [[nodiscard]] T &get(size_t id)
    {
        return m_storage[id];
    }

    void for_each(auto f)
    {
        for (auto &x : m_storage)
        {
            f(x);
        }
    }
};
