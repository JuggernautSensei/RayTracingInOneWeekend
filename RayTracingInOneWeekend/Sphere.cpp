#include "pch.h"
#include "Sphere.h"
#include "Interval.h"
#include "Material.h"

Sphere::Sphere(
    const Vec3                       _center,
    const float                      _radius,
    const std::shared_ptr<Material>& _pMaterial)
    : m_center(_center)
    , m_radius(_radius)
    , m_pMaterial(_pMaterial)
{
    ASSERT(_pMaterial, "Material pointer must not be null.");
}

void Sphere::SetCenter(
    const Vec3 _center)
{
    m_center = _center;
}

void Sphere::SetRadius(
    const float _radius)
{
    m_radius = _radius;
}

void Sphere::SetMaterial(
    const std::shared_ptr<Material>& _pMaterial)
{
    ASSERT(_pMaterial, "Material pointer must not be null.");
    m_pMaterial = _pMaterial;
}

Vec3 Sphere::GetCenter() const
{
    return m_center;
}

float Sphere::GetRadius() const
{
    return m_radius;
}

const std::shared_ptr<Material>& Sphere::GetMaterial() const
{
    return m_pMaterial;
}

bool Sphere::Hit(
    const Ray&     _ray,
    const Interval _interval,
    HitRecord&     _record) const
{
    const Vec3  u  = _ray.origin - m_center;
    const float vu = _ray.dir.Dot(u);
    const float a  = _ray.dir.LengthSq();
    const float c  = u.LengthSq() - m_radius * m_radius;
    const float d  = vu * vu - a * c;   // 판별식

    if (d < 0.f)   // 근이 존재하지 않음
    {
        return false;
    }

    const float sqrt = std::sqrtf(d);

    float t = (-vu - sqrt) / a;   // 가까운 근 체크
    if (!_interval.Surrounds(t))
    {
        t = (-vu + sqrt) / a;   // 먼 근 체크
        if (!_interval.Surrounds(t))
        {
            return false;
        }
    }

    _record.t          = t;
    _record.point      = _ray.At(t);
    _record.normal     = (_record.point - m_center) / m_radius;
    _record.bFrontFace = true;
    _record.pMaterial  = m_pMaterial.get();

    // back face 충돌
    if (_ray.dir.Dot(_record.normal) > 0.f)
    {
        _record.normal     = -_record.normal;
        _record.bFrontFace = false;
    }

    return true;
}