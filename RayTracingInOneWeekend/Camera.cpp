#include "pch.h"
#include "Camera.h"

#include <thread>

#include "IHittable.h"
#include "Interval.h"
#include "RandomUtils.h"

namespace
{
[[nodiscard]] Vec3 GenerateRandomUnitVecOnSphere()
{
    while (true)
    {
        Vec3        vec   = { GenerateRandomSHR3<float>(-1.f, 1.f), GenerateRandomSHR3<float>(-1.f, 1.f), GenerateRandomSHR3<float>(-1.f, 1.f) };
        const float lenSq = vec.LengthSq();
        if (lenSq >= 1.f || IsZeroApprox(lenSq))
        {
            continue;
        }

        vec = vec / ::sqrtf(lenSq);
        return vec;
    }
}

[[nodiscard]] Vec3 GammaCorrection(
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

[[nodiscard]] Vec3 GenerateRandomUnitVec3OnHemisphere(
    const Vec3 _normal)
{
    const Vec3 vec = GenerateRandomUnitVecOnSphere();
    return vec.Dot(_normal) > 0.f ? vec : -vec;
}

}   // namespace

Camera::Camera(
    const Int32 _width,
    const Int32 _height)
    : m_width(_width)
    , m_height(_height)
{
}

void Camera::SetWidth(
    const Int32 _width)
{
    if (m_width == _width)
    {
        return;
    }

    m_width         = _width;
    m_bPaletteDirty = true;
}

void Camera::SetHeight(
    const Int32 _height)
{
    if (m_height == _height)
    {
        return;
    }

    m_height        = _height;
    m_bPaletteDirty = true;
}

void Camera::SetCenter(
    const Vec3 _center)
{
    m_center = _center;
}

void Camera::SetFOV(
    const float _fovRad)
{
    if (::IsEqualApprox(m_fovRad, _fovRad))
    {
        return;
    }

    m_fovRad        = _fovRad;
    m_bPaletteDirty = true;
}

void Camera::SetSample(
    const Int32 _sample)
{
    m_sample = _sample;
}

Int32 Camera::GetWidth() const
{
    return m_width;
}

Int32 Camera::GetHeight() const
{
    return m_height;
}

Vec3 Camera::GetCenter() const
{
    return m_center;
}

float Camera::GetFOV() const
{
    return m_fovRad;
}

Int32 Camera::GetSample() const
{
    return m_sample;
}

void Camera::Render(const IHittable& _world)
{
    FUNCTION_PROFILE();
    UpdatePaletteIfNeed_();

    const float        viewportWidthHalf = m_viewportWidth * 0.5f;
    std::atomic<Int32> completed         = 0;

#pragma omp parallel for schedule(dynamic)
    for (Int32 y = 0; y < m_height; ++y)
    {
        for (Int32 x = 0; x < m_width; ++x)
        {
            Vec3 color = Vec3::kZero;
            if (m_sample == 1)   // non-anti-aliasing
            {
                const float yy     = kViewportHeightHalf - (static_cast<float>(y) + 0.5f) * m_pixelH;
                const float xx     = -viewportWidthHalf + (static_cast<float>(x) + 0.5f) * m_pixelW;
                const Vec3  pixel  = { xx, yy, m_focalLength };
                const Vec3  origin = (pixel - m_center).Normalize();
                const Ray   ray    = { m_center, origin };
                color              = RayColor_(ray, 0, _world);
            }
            else   // anti-aliasing
            {
                for (int i = 0; i < m_sample; ++i)
                {
                    const Vec3  offset = { GenerateRandomSHR3<float>(0.f, 1.f), GenerateRandomSHR3<float>(0.f, 1.f), 0.f };
                    const float xx     = -viewportWidthHalf + (static_cast<float>(x) + offset.x) * m_pixelW;
                    const float yy     = kViewportHeightHalf - (static_cast<float>(y) + offset.y) * m_pixelH;
                    const Vec3  pixel  = { xx, yy, m_focalLength };
                    const Vec3  origin = (pixel - m_center).Normalize();
                    const Ray   ray    = { m_center, origin };
                    color += RayColor_(ray, 0, _world);
                }
                color /= static_cast<float>(m_sample);
            }

            m_imageBuffer.WriteLinear(x, y, GammaCorrection(color, 2.f));
        }

        const Int32 done = ++completed;
        if (done % 10 == 0 || done == m_height)
        {
#pragma omp critical
            {
                const float progress = static_cast<float>(done) / static_cast<float>(m_height);
                std::cout << "\rRendering: [" << done * m_width << "/" << m_height * m_width << "] " << 100.f * progress << "%      " << std::flush;
            }
        }
    }

    std::cout << "\n";
}

const RGBImageBuffer& Camera::GetImageBuffer() const
{
    return m_imageBuffer;
}

void Camera::UpdatePaletteIfNeed_()
{
    if (!m_bPaletteDirty)
    {
        return;
    }

    const float aspectRatio = static_cast<float>(m_width) / static_cast<float>(m_height);

    m_viewportWidth = kViewportHeight * aspectRatio;
    m_pixelW        = m_viewportWidth / static_cast<float>(m_width);
    m_pixelH        = kViewportHeight / static_cast<float>(m_height);
    m_focalLength   = kViewportHeight / (2.f * ::tanf(m_fovRad * 0.5f));
    m_imageBuffer   = { m_width, m_height };
    m_bPaletteDirty = false;
}

Vec3 Camera::RayColor_(
    const Ray&       _ray,
    const Int32      _depth,
    const IHittable& _world)
{
    if (_depth > m_maxDepth)
    {
        return Vec3::kZero;
    }

    HitRecord      record   = {};
    const Interval interval = { 0.001f, Max<float>() };
    if (_world.Hit(_ray, interval, record))
    {
        const Vec3 dir = (record.normal + GenerateRandomUnitVecOnSphere()).Normalize();
        const Ray  ray = { record.point, dir };
        return 0.5f * RayColor_(ray, _depth + 1, _world);
    }

    constexpr Vec3 kWhite { 1.f };
    constexpr Vec3 kSky = { 0.5f, 0.7f, 1.f };
    const float    a    = 0.5f * (_ray.dir.y + 1.f);
    return kWhite.Lerp(kSky, a);
}
