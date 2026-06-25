#include "pch.h"
#include "HittableList.h"
#include "Interval.h"

void HittableList::Clear()
{
    m_hittables.clear();
}

void HittableList::Add(
    std::unique_ptr<IHittable>&& _pHittable)
{
    m_hittables.emplace_back(std::move(_pHittable));
}

bool HittableList::Hit(
    const Ray& _ray,
    Interval   _interval,
    HitRecord& _outRecord) const
{
    bool bAnyHit = false;

    for (const std::unique_ptr<IHittable>& pHittable: m_hittables)
    {
        if (pHittable->Hit(_ray, _interval, _outRecord))
        {
            bAnyHit       = true;
            _interval.max = _outRecord.t;   // 가장 가까운 hit만 기록
        }
    }

    return bAnyHit;
}