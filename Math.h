#pragma once

constexpr float kPI      = 3.14159265358979323846f;
constexpr float k2PI     = 2.f * kPI;
constexpr float kInvPI   = 0.5f * kPI;
constexpr float kDeg2Rad = kPI / 180.f;
constexpr float kRad2Deg = 180.f / kPI;
constexpr float kEpsilon = 1e-6f;

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