#pragma once
#include "Material.h"

class Lambertian : public Material
{
public:
    explicit Lambertian(Vec3 _albedo);

    void               SetAlbedo(Vec3 _albedo);
    [[nodiscard]] Vec3 GetAlbedo() const;

    bool Scatter(const Ray& _ray, const HitRecord& _record, Vec3& _attenuation, Ray& _scattered) const override;

private:
    Vec3 m_albedo = Vec3::kZero;
};
