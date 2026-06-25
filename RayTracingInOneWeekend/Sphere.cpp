#include "pch.h"
#include "Sphere.h"
#include "Interval.h"

Sphere::Sphere(
    const Vec3  _center,
    const float _radius)
    : m_center(_center)
    , m_radius(_radius)
{
}

void Sphere::SetCenter(const Vec3 _center)
{
    m_center = _center;
}

void Sphere::SetRadius(const float _radius)
{
    m_radius = _radius;
}

Vec3 Sphere::GetCenter() const
{
    return m_center;
}

float Sphere::GetRadius() const
{
    return m_radius;
}

bool Sphere::Hit(const Ray& _ray, const Interval _interval, HitRecord& _record) const
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

    // back face 충돌
    if (_ray.dir.Dot(_record.normal) > 0.f)
    {
        _record.normal     = -_record.normal;
        _record.bFrontFace = false;
    }

    return true;
}