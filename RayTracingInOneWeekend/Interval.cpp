#include "pch.h"
#include "Interval.h"

const Interval Interval::kEmpty    = { Max<float>(), Min<float>() };
const Interval Interval::kUniverse = { Min<float>(), Max<float>() };

Interval::Interval(
    const float _min,
    const float _max)
    : min(_min)
    , max(_max)
{
}

float Interval::Size() const
{
    return max - min;
}

bool Interval::Contains(
    const float _value) const
{
    return min <= _value && _value <= max;
}

bool Interval::Surrounds(
    const float _value) const
{
    return min < _value && _value < max;
}

float Interval::Clamp(
    const float _value) const
{
    return ::Clamp(_value, min, max);
}