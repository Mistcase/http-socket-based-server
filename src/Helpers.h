#pragma once

#include <cstddef>

namespace helpers
{
    template <typename T, size_t N>
    constexpr size_t GetArraySize(T (&)[N])
    {
        return N;
    }

} // namespace helpers
