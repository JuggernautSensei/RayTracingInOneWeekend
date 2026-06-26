#include "pch.h"
#include "VectorUtils.h"

#include "RandomUtils.h"

Vec3 GenerateRandomUnitVecOnSphere()
{
    while (true)
    {
        Vec3 vec = {
            GenerateRandomSHR3<float>(-1.f, 1.f),
            GenerateRandomSHR3<float>(-1.f, 1.f),
            GenerateRandomSHR3<float>(-1.f, 1.f)
        };
        const float lenSq = vec.LengthSq();
        if (lenSq >= 1.f || IsZeroApprox(lenSq))
        {
            continue;
        }

        vec = vec / ::sqrtf(lenSq);
        return vec;
    }
}

Vec3 GammaCorrect(
    const Vec3  _linearColor,
    const float _gamma)
{
    const float invGamma = 1.f / _gamma;
    return {
        ::powf(_linearColor.r, invGamma),
        ::powf(_linearColor.g, invGamma),
        ::powf(_linearColor.b, invGamma)
    };
}

Vec3 GenerateRandomUnitVec3OnHemisphere(
    const Vec3 _normal)
{
    const Vec3 vec = GenerateRandomUnitVecOnSphere();
    return vec.Dot(_normal) > 0.f ? vec : -vec;
}