#include "pch.h"
#include "Dielectric.h"

#include "IHittable.h"
#include "RandomUtils.h"

namespace
{

float Reflectance(
    const float _cosine,
    const float _ior)
{
    float r0 = (1.f - _ior) / (1.f + _ior);
    r0 *= r0;
    return r0 + (1.f - r0) * ::powf(1.f - _cosine, 5.f);
}

}   // namespace

Dielectric::Dielectric(
    const float _refractionIndex)
    : m_ior(_refractionIndex)
{
}

bool Dielectric::Scatter(
    const Ray&       _ray,
    const HitRecord& _record,
    Vec3&            _attenuation,
    Ray&             _scattered) const
{
    ASSERT(_ray.dir.IsNormalized(), "Ray direction must be normalized.");

    _attenuation    = Vec3::kOne;
    const float eta = _record.bFrontFace ? (1.f / m_ior) : m_ior;
    const float cos = Min(-_ray.dir.Dot(_record.normal), 1.f);
    const float sin = ::sqrtf(1.f - cos * cos);

    Vec3 dir = Vec3::kZero;
    if (eta * sin > 1.f || Reflectance(cos, eta) > GenerateRandomSHR3<float>(0.f, 1.f))
    {
        // cannot refract
        dir = _ray.dir.Reflect(_record.normal).Normalize();
    }
    else
    {
        dir = _ray.dir.Refract(_record.normal, eta).Normalize();
    }

    _scattered = { _record.point, dir };
    return true;
}
