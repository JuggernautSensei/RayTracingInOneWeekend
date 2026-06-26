#include "pch.h"
#include "Lambertian.h"

#include "VectorUtils.h"
#include "IHittable.h"

Lambertian::Lambertian(
    const Vec3 _albedo)
    : m_albedo(_albedo)
{
}

void Lambertian::SetAlbedo(
    const Vec3 _albedo)
{
    m_albedo = _albedo;
}

Vec3 Lambertian::GetAlbedo() const
{
    return m_albedo;
}

bool Lambertian::Scatter(
    const Ray&       _ray,
    const HitRecord& _record,
    Vec3&            _attenuation,
    Ray&             _scattered) const
{
    const Vec3 dir = (_record.normal + GenerateRandomUnitVecOnSphere()).Normalize();
    _scattered     = { _record.point, dir };
    _attenuation   = m_albedo;
    return true;
}