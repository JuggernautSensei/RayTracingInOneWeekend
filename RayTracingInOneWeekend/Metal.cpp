#include "pch.h"
#include "Metal.h"

#include "IHittable.h"
#include "VectorUtils.h"

Metal::Metal(
    const Vec3  _albedo,
    const float _fuzz)
    : m_albedo(_albedo)
    , m_fuzz(_fuzz)
{
    ASSERT(m_fuzz >= 0.f && m_fuzz <= 1.f, "Fuzz must be in the range of [0, 1].");
}

bool Metal::Scatter(
    const Ray&       _ray,
    const HitRecord& _record,
    Vec3&            _attenuation,
    Ray&             _scattered) const
{
    const Vec3 reflected = (_ray.dir.Reflect(_record.normal) + GenerateRandomUnitVecOnSphere() * m_fuzz).Normalize();
    _scattered           = { _record.point, reflected };
    _attenuation         = m_albedo;
    return reflected.Dot(_record.normal) > 0.f;
}

void Metal::SetAlbedo(
    const Vec3 _albedo)
{
    m_albedo = _albedo;
}

Vec3 Metal::GetAlbedo() const
{
    return m_albedo;
}