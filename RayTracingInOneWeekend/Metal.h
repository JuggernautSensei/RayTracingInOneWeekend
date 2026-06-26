#pragma once
#include "Material.h"

class Metal : public Material
{
public:
    explicit Metal(Vec3 _albedo, float _fuzz);
    bool Scatter(const Ray& _ray, const HitRecord& _record, Vec3& _attenuation, Ray& _scattered) const override;

    void               SetAlbedo(Vec3 _albedo);
    [[nodiscard]] Vec3 GetAlbedo() const;

private:
    Vec3  m_albedo = Vec3::kZero;
    float m_fuzz   = 0.f;
};
