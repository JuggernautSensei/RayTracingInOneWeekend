#pragma once

struct Interval;

struct HitRecord
{
    Vec3  point      = Vec3::kZero;
    Vec3  normal     = Vec3::kZero;
    float t          = 0.f;
    bool  bFrontFace = false;
};

class IHittable
{
public:
    IHittable()          = default;
    virtual ~IHittable() = default;

    IHittable(IHittable&&)                 = default;
    IHittable(const IHittable&)            = default;
    IHittable& operator=(IHittable&&)      = default;
    IHittable& operator=(const IHittable&) = default;

    virtual bool Hit(const Ray& _ray, Interval _interval, HitRecord& _outRecord) const = 0;
};