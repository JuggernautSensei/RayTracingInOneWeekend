#pragma once
#include "Random.h"

namespace Details
{

extern thread_local RandomGeneratorSHR3 g_rngSHR3;

}

template<typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] T GenerateRandomSHR3(const T _min, const T _max) noexcept
{
    return GenerateRandom<T>(Details::g_rngSHR3, _min, _max);
}
