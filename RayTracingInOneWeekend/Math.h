#pragma once

constexpr float kPI      = 3.14159265358979323846f;
constexpr float k2PI     = 2.f * kPI;
constexpr float kInvPI   = 0.5f * kPI;
constexpr float kDeg2Rad = kPI / 180.f;
constexpr float kRad2Deg = 180.f / kPI;
constexpr float kEpsilon = 1e-6f;

template<typename T>
struct Limits
{
};

template<>
struct Limits<bool>
{
    static constexpr bool kMin = false;
    static constexpr bool kMax = true;
};

template<>
struct Limits<double>
{
    static constexpr double kMin = -DBL_MAX;
    static constexpr double kMax = DBL_MAX;
};

template<>
struct Limits<float>
{
    static constexpr float kMin = -FLT_MAX;
    static constexpr float kMax = FLT_MAX;
};

template<>
struct Limits<Int32>
{
    static constexpr Int32 kMin = INT32_MIN;
    static constexpr Int32 kMax = INT32_MAX;
};

template<>
struct Limits<UInt32>
{
    static constexpr UInt32 kMin = 0;
    static constexpr UInt32 kMax = UINT32_MAX;
};

template<>
struct Limits<Int64>
{
    static constexpr Int64 kMin = INT64_MIN;
    static constexpr Int64 kMax = INT64_MAX;
};

template<>
struct Limits<UInt64>
{
    static constexpr UInt64 kMin = 0;
    static constexpr UInt64 kMax = UINT64_MAX;
};

template<>
struct Limits<UInt16>
{
    static constexpr UInt16 kMin = 0;
    static constexpr UInt16 kMax = UINT16_MAX;
};

template<>
struct Limits<Int16>
{
    static constexpr Int16 kMin = INT16_MIN;
    static constexpr Int16 kMax = INT16_MAX;
};

template<>
struct Limits<UInt8>
{
    static constexpr UInt8 kMin = 0;
    static constexpr UInt8 kMax = UINT8_MAX;
};

template<>
struct Limits<Int8>
{
    static constexpr Int8 kMin = INT8_MIN;
    static constexpr Int8 kMax = INT8_MAX;
};

template<typename T>
[[nodiscard]] constexpr T Max()
{
    return Limits<T>::kMax;
}

template<typename T>
[[nodiscard]] constexpr T Min()
{
    return Limits<T>::kMin;
}

template<typename T>
[[nodiscard]] constexpr float Min(
    const float _x,
    const float _y)
{
    return _x < _y ? _x : _y;
}

template<typename T>
[[nodiscard]] constexpr float Max(
    const float _x,
    const float _y)
{
    return _x > _y ? _x : _y;
}

template<typename T, typename... Args>
[[nodiscard]] constexpr T Min(
    const T _x,
    const T _y,
    const Args... _args)
{
    return Min(Min(_x, _y), _args...);
}

template<typename T, typename... Args>
[[nodiscard]] constexpr T Max(
    const T _x,
    const T _y,
    const Args... _args)
{
    return Max(Max(_x, _y), _args...);
}

[[nodiscard]] constexpr float ToDeg(
    const float _rad)
{
    return _rad * kRad2Deg;
}

[[nodiscard]] constexpr float ToRad(
    const float _deg)
{
    return _deg * kDeg2Rad;
}

[[nodiscard]] constexpr float Abs(
    const float _x)
{
    return _x < 0.f ? -_x : _x;
}

[[nodiscard]] constexpr bool IsZeroApprox(
    const float _x)
{
    return Abs(_x) < kEpsilon;
}

[[nodiscard]] constexpr bool IsEqualApprox(
    const float _x,
    const float _y)
{
    return IsZeroApprox(_x - _y);
}

[[nodiscard]] constexpr float Lerp(
    const float _a,
    const float _b,
    const float _t)
{
    return _a + (_b - _a) * _t;
}

[[nodiscard]] constexpr float Trunc(
    const float _x)
{
    return static_cast<float>(static_cast<int>(_x));
}

[[nodiscard]] constexpr float Fract(
    const float _x)
{
    return _x - Trunc(_x);
}

[[nodiscard]] constexpr float Floor(
    const float _x)
{
    if (_x < 0.f)
    {
        const float fr = Fract(-_x);
        const float tr = Trunc(-_x);
        return -tr - (fr != 0.f ? 1.f : 0.f);
    }
    else
    {
        return Trunc(_x);
    }
}

[[nodiscard]] constexpr float Ceil(
    const float _x)
{
    return -Floor(-_x);
}

[[nodiscard]] constexpr float Round(
    const float _x)
{
    return _x < 0.f ? Ceil(_x - 0.5f) : Floor(_x + 0.5f);
}

[[nodiscard]] constexpr float Clamp(
    const float _x,
    const float _min,
    const float _max)
{
    return _x < _min ? _min : (_x > _max ? _max : _x);
}
