#include "pch.h"
#include "Camera.h"

#include "IHittable.h"
#include "Interval.h"

#include <omp.h>
#include <thread>

Camera::Camera(
    const Int32 _width,
    const Int32 _height,
    const Vec3  _center,
    const float _fovRad)
    : m_width(_width)
    , m_height(_height)
    , m_center(_center)
    , m_fovRad(_fovRad)
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

void Camera::Render(const IHittable& _world)
{
    FUNCTION_PROFILE();
    UpdatePaletteIfNeed_();

    const float viewportWidthHalf = m_viewportWidth * 0.5f;

#pragma omp parallel for schedule(dynamic)
    for (Int32 y = 0; y < m_height; ++y)
    {
        for (Int32 x = 0; x < m_width; ++x)
        {
            const float yy  = kViewportHeightHalf - (static_cast<float>(y) + 0.5f) * m_pixelH;
            const float xx  = -viewportWidthHalf + (static_cast<float>(x) + 0.5f) * m_pixelW;
            const Vec3  dir = (Vec3 { xx, yy, m_focalLength } - m_center).Normalize();
            const Ray   ray = { m_center, dir };
            m_imageBuffer.WriteLinear(x, y, RayColor_(ray, _world));
        }
    }
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
    const IHittable& _world) const
{
    HitRecord      record   = {};
    const Interval interval = { 0.f, Max<float>() };
    if (_world.Hit(_ray, interval, record))
    {
        return record.normal * 0.5f + Vec3 { 0.5f };
    }

    constexpr Vec3 kWhite { 1.f };
    constexpr Vec3 kSky = { 0.5f, 0.7f, 1.f };
    const float    a    = 0.5f * (_ray.dir.y + 1.f);
    return kWhite.Lerp(kSky, a);
}
