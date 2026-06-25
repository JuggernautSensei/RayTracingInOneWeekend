#pragma once

struct Interval
{
    Interval() = default;
    Interval(float _min, float _max);

    [[nodiscard]] float Size() const;
    [[nodiscard]] bool  Contains(float _value) const;
    [[nodiscard]] bool  Surrounds(float _value) const;

    static const Interval kEmpty;
    static const Interval kUniverse;

    float min = Max<float>();
    float max = Min<float>();
};
