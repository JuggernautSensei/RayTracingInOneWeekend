#pragma once
#include "IHittable.h"

class Sphere : public IHittable
{
public:
    Sphere() = default;
    Sphere(Vec3 _center, float _radius);

    void SetCenter(Vec3 _center);
    void SetRadius(float _radius);

    [[nodiscard]] Vec3  GetCenter() const;
    [[nodiscard]] float GetRadius() const;

    bool Hit(const Ray& _ray, Interval _interval, HitRecord& _record) const override;

private:
    Vec3  m_center = Vec3::kZero;
    float m_radius = 1.f;
};
