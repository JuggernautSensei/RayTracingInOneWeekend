#pragma once
#include "Math.h"

struct Vec3
{
    constexpr Vec3() = default;

    constexpr Vec3(
        const float _x,
        const float _y,
        const float _z)
        : x(_x)
        , y(_y)
        , z(_z)
    {
    }

    constexpr explicit Vec3(
        const float _xyz)
        : x(_xyz)
        , y(_xyz)
        , z(_xyz)
    {
    }

    constexpr Vec3& operator*=(
        const float _scalar)
    {
        x *= _scalar;
        y *= _scalar;
        z *= _scalar;
        return *this;
    }

    constexpr Vec3& operator/=(
        const float _scalar)
    {
        float inv = 1.f / _scalar;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }

    constexpr Vec3& operator+=(
        const Vec3 _other)
    {
        x += _other.x;
        y += _other.y;
        z += _other.z;
        return *this;
    }

    constexpr Vec3& operator-=(
        const Vec3 _other)
    {
        x -= _other.x;
        y -= _other.y;
        z -= _other.z;
        return *this;
    }

    [[nodiscard]] constexpr Vec3 operator+(
        const Vec3 _other) const
    {
        Vec3 v = *this;
        v.x += _other.x;
        v.y += _other.y;
        v.z += _other.z;
        return v;
    }

    [[nodiscard]] constexpr Vec3 operator-(
        const Vec3 _other) const
    {
        Vec3 v = *this;
        v.x -= _other.x;
        v.y -= _other.y;
        v.z -= _other.z;
        return v;
    }

    [[nodiscard]] constexpr Vec3 operator*(
        const float _scalar) const
    {
        Vec3 v = *this;
        v.x *= _scalar;
        v.y *= _scalar;
        v.z *= _scalar;
        return v;
    }

    [[nodiscard]] constexpr Vec3 operator/(
        const float _scalar) const
    {
        Vec3 v = *this;

        const float inv = 1.f / _scalar;
        v.x *= inv;
        v.y *= inv;
        v.z *= inv;
        return v;
    }

    [[nodiscard]] constexpr Vec3 operator-() const
    {
        return { -x, -y, -z };
    }

    [[nodiscard]] constexpr Vec3 operator*(
        const Vec3 _other) const
    {
        return { x * _other.x, y * _other.y, z * _other.z };
    }

    [[nodiscard]] constexpr Vec3 operator/(
        const Vec3 _other) const
    {
        return { x / _other.x, y / _other.y, z / _other.z };
    }

    [[nodiscard]] constexpr float Dot(
        const Vec3 _other) const
    {
        return x * _other.x + y * _other.y + z * _other.z;
    }

    [[nodiscard]] constexpr Vec3 Cross(
        const Vec3 _other) const
    {
        return {
            y * _other.z - z * _other.y,
            z * _other.x - x * _other.z,
            x * _other.y - y * _other.x
        };
    }

    [[nodiscard]] constexpr Vec3 Lerp(
        const Vec3  _other,
        const float _t) const
    {
        return {
            ::Lerp(x, _other.x, _t),
            ::Lerp(y, _other.y, _t),
            ::Lerp(z, _other.z, _t)
        };
    }

    [[nodiscard]] constexpr float LengthSq() const
    {
        return Dot(*this);
    }

    [[nodiscard]] float Length() const
    {
        return ::sqrtf(LengthSq());
    }

    [[nodiscard]] Vec3 Normalize() const
    {
        const float lenSq = LengthSq();
        if (::IsZeroApprox(lenSq))
        {
            return Vec3::kZero;
        }

        const float inv = 1.f / ::sqrtf(lenSq);
        return { *this * inv };
    }

    [[nodiscard]] bool IsNormalized() const
    {
        return ::IsEqualApprox(LengthSq(), 1.f);
    }

    [[nodiscard]] constexpr bool IsZeroApprox() const
    {
        return ::IsZeroApprox(x)
            && ::IsZeroApprox(y)
            && ::IsZeroApprox(z);
    }

    [[nodiscard]] constexpr bool IsEqualApprox(
        const Vec3 _other) const
    {
        return ::IsEqualApprox(x, _other.x)
            && ::IsEqualApprox(y, _other.y)
            && ::IsEqualApprox(z, _other.z);
    }

    [[nodiscard]] Vec3 Clamp(
        const Vec3 _min,
        const Vec3 _max) const
    {
        return {
            std::clamp(x, _min.x, _max.x),
            std::clamp(y, _min.y, _max.y),
            std::clamp(z, _min.z, _max.z)
        };
    }

    [[nodiscard]] constexpr Vec3 Reflect(
        const Vec3 _normal) const
    {
        return *this - _normal * 2.f * Dot(_normal);
    }

    static Vec3 kZero;
    static Vec3 kOne;
    static Vec3 kUnitX;
    static Vec3 kUnitY;
    static Vec3 kUnitZ;
    static Vec3 kRight;
    static Vec3 kUp;
    static Vec3 kForward;

    union
    {
        struct
        {
            float x, y, z;
        };

        struct
        {
            float r, g, b;
        };

        std::array<float, 3> e = {};
    };
};

inline Vec3 operator*(
    const float _scalar,
    const Vec3  _v)
{
    return _v * _scalar;
}

inline Vec3 operator/(
    const float _scalar,
    const Vec3  _v)
{
    return _v / _scalar;
}
