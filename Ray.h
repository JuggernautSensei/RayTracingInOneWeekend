#pragma once

struct Ray
{
    Ray() = default;
    Ray(
        const Vec3 _origin,
        const Vec3 _dir)
        : origin(_origin)
        , dir(_dir)
    {
        ASSERT(dir.IsNormalized(), "Ray direction must be normalized.");
    }

    [[nodiscard]] Vec3 At(const float _t) const
    {
        ASSERT(dir.IsNormalized(), "Ray direction must be normalized.");
        return origin + dir * _t;
    }

    Vec3 origin = Vec3::kZero;
    Vec3 dir    = Vec3::kUnitZ;
};
