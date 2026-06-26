#pragma once
#include "IHittable.h"

class Sphere : public IHittable
{
public:
    Sphere(Vec3 _center, float _radius, const std::shared_ptr<Material>& _pMaterial);

    void SetCenter(Vec3 _center);
    void SetRadius(float _radius);
    void SetMaterial(const std::shared_ptr<Material>& _pMaterial);

    [[nodiscard]] Vec3      GetCenter() const;
    [[nodiscard]] float     GetRadius() const;
    [[nodiscard]] const std::shared_ptr<Material>& GetMaterial() const;

    bool Hit(const Ray& _ray, Interval _interval, HitRecord& _record) const override;

private:
    Vec3                      m_center    = Vec3::kZero;
    float                     m_radius    = 1.f;
    std::shared_ptr<Material> m_pMaterial = nullptr;
};
