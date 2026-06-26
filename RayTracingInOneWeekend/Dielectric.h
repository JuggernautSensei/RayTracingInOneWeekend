#pragma once
#include "Material.h"

class Dielectric : public Material
{
public:
    explicit Dielectric(float _refractionIndex);
    bool Scatter(const Ray& _ray, const HitRecord& _record, Vec3& _attenuation, Ray& _scattered) const override;

private:
    float m_ior = 1.0f;
};
