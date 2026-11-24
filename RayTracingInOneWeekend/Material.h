#pragma once

struct HitRecord;

class Material
{
public:
    Material()          = default;
    virtual ~Material() = default;

    Material& operator=(const Material&) = default;
    Material(const Material&)            = default;
    Material& operator=(Material&&)      = default;
    Material(Material&&)                 = default;

    virtual bool Scatter(const Ray& _ray, const HitRecord& _record, Vec3& _attenuation, Ray& _scattered) const = 0;
};
