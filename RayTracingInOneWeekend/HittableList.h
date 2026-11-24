#pragma once
#include "IHittable.h"

class HittableList : public IHittable
{
public:
    HittableList() = default;

    void Clear();
    void Add(std::unique_ptr<IHittable>&& _pHittable);
    bool Hit(const Ray& _ray, Interval _interval, HitRecord& _outRecord) const override;

private:
    std::vector<std::unique_ptr<IHittable>> m_hittables;
};