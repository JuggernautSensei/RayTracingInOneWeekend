#include "pch.h"
#include "Camera.h"

#include "IHittable.h"
#include "Interval.h"
#include "RandomUtils.h"
#include "Material.h"
#include "VectorUtils.h"

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

    m_width        = _width;
    m_bCameraDirty = true;
}

void Camera::SetHeight(
    const Int32 _height)
{
    if (m_height == _height)
    {
        return;
    }

    m_height       = _height;
    m_bCameraDirty = true;
}

void Camera::SetCenter(
    const Vec3 _center)
{
    if (m_center.IsEqualApprox(_center))
    {
        return;
    }

    m_center       = _center;
    m_bCameraDirty = true;
}

void Camera::SetFOV(
    const float _rad)
{
    if (::IsEqualApprox(m_fovRad, _rad))
    {
        return;
    }

    m_fovRad       = _rad;
    m_bCameraDirty = true;
}

void Camera::SetSample(
    const Int32 _sample)
{
    ASSERT(_sample > 0, "Camera sample must be greater than 0.");
    m_sample = _sample;
}

void Camera::SetLookTo(
    const Vec3 _dir)
{
    ASSERT(_dir.IsNormalized(), "Camera look direction must be normalized.");
    if (m_lookTo.IsEqualApprox(_dir))
    {
        return;
    }

    m_lookTo       = _dir;
    m_bCameraDirty = true;
}

void Camera::SetLookAt(
    const Vec3 _point)
{
    ASSERT(!_point.IsEqualApprox(m_center), "Camera look at point cannot be the same as camera center.");
    const Vec3 dir = (_point - m_center).Normalize();
    SetLookTo(dir);
}

void Camera::SetCameraUp(
    const Vec3 _up)
{
    ASSERT(_up.IsNormalized(), "Camera up direction must be normalized.");
    if (m_up.IsEqualApprox(_up))
    {
        return;
    }

    m_up           = _up;
    m_bCameraDirty = true;
}

void Camera::SetDefocusRad(
    const float _defocusRad)
{
    if (IsEqualApprox(m_defocusRad, _defocusRad))
    {
        return;
    }

    m_defocusRad   = _defocusRad;
    m_bCameraDirty = true;
}

void Camera::SetFocusDistance(
    const float _dist)
{
    if (IsEqualApprox(m_focusDist, _dist))
    {
        return;
    }

    m_focusDist    = _dist;
    m_bCameraDirty = true;
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

Vec3 Camera::GetLookTo() const
{
    return m_lookTo;
}

Vec3 Camera::GetCameraUp() const
{
    return m_up;
}

float Camera::GetDefocusRad() const
{
    return m_defocusRad;
}

float Camera::GetFocusDistance() const
{
    return m_focusDist;
}

float Camera::GetFOV() const
{
    return m_fovRad;
}

Int32 Camera::GetSample() const
{
    return m_sample;
}

void Camera::Render(
    const IHittable& _world)
{
    ASSERT(m_sample > 0, "Camera sample must be greater than 0.");

    FUNCTION_PROFILE();
    UpdateDirty_();

    const float        viewportWidthHalf = m_viewportWidth * 0.5f;
    std::atomic<Int32> completed         = 0;

#pragma omp parallel for schedule(dynamic)
    for (Int32 y = 0; y < m_height; ++y)
    {
        for (Int32 x = 0; x < m_width; ++x)
        {
            Vec3 color = Vec3::kZero;
            for (int i = 0; i < m_sample; ++i)
            {
                const Ray ray = CalcRay_(x, y);
                color += RayColor_(ray, 0, _world);
            }
            color /= static_cast<float>(m_sample);
            m_imageBuffer.WriteLinear(x, y, GammaCorrect(color, 2.f));
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

void Camera::UpdateDirty_()
{
    if (!m_bCameraDirty)
    {
        return;
    }

    m_imageBuffer = { m_width, m_height };

    const float aspectRatio = static_cast<float>(m_width) / static_cast<float>(m_height);
    m_viewportHeight        = 2 * ::tanf(m_fovRad * 0.5f) * m_focusDist;
    m_viewportWidth         = m_viewportHeight * aspectRatio;

    ASSERT(!m_lookTo.IsEqualApprox(m_up), "Camera look direction and up direction cannot be the same.");
    ASSERT(m_up.IsNormalized() && m_lookTo.IsNormalized(), "Camera look direction and up direction must be normalized.");

    const Vec3 right = m_up.Cross(m_lookTo).Normalize();
    const Vec3 up    = m_lookTo.Cross(right).Normalize();

    const float pixelW = m_viewportWidth / static_cast<float>(m_width);
    const float pixelH = m_viewportHeight / static_cast<float>(m_height);
    m_pixelDeltaRight  = right * pixelW;
    m_pixelDeltaUp     = -up * pixelH;
    m_pixelStartPosW   = m_center
                       + m_lookTo * m_focusDist
                       - right * (m_viewportWidth * 0.5f)
                       + up * (m_viewportHeight * 0.5f);

    const float defocusRadius = m_focusDist * ::tanf(m_defocusRad * 0.5f);
    m_defocusDiskRight        = right * defocusRadius;
    m_defocusDiskUp           = up * defocusRadius;

    m_bCameraDirty = false;
}

Vec3 Camera::RayColor_(
    const Ray&       _ray,
    const Int32      _depth,
    const IHittable& _world)
{
    if (_depth > m_maxDepth)
    {
        return Vec3::kOne;
    }

    HitRecord      record   = {};
    const Interval interval = { 0.001f, Max<float>() };
    if (_world.Hit(_ray, interval, record))
    {
        Ray  scattered   = {};
        Vec3 attenuation = {};
        if (record.pMaterial->Scatter(_ray, record, attenuation, scattered))
        {
            return attenuation * RayColor_(scattered, _depth + 1, _world);
        }
        return Vec3::kZero;
    }

    constexpr Vec3 kWhite { 1.f };
    constexpr Vec3 kSky = { 0.5f, 0.7f, 1.f };
    const float    a    = 0.5f * (_ray.dir.y + 1.f);
    return kWhite.Lerp(kSky, a);
}

Ray Camera::CalcRay_(
    const Int32 _xScreen,
    const Int32 _yScreen) const
{
    const float xOffset = m_sample == 1 ? 0.5f : GenerateRandomSHR3<float>(0.f, 1.f);
    const float yOffset = m_sample == 1 ? 0.5f : GenerateRandomSHR3<float>(0.f, 1.f);
    const Vec3  pixel   = m_pixelStartPosW
                        + m_pixelDeltaRight * (static_cast<float>(_xScreen) + xOffset)
                        + m_pixelDeltaUp * (static_cast<float>(_yScreen) + yOffset);

    const Vec3 origin = IsZeroApprox(m_defocusRad) ? m_center : GenerateDefocusDiskSample_();
    const Vec3 dir    = (pixel - origin).Normalize();
    return { origin, dir };
}

Vec3 Camera::GenerateDefocusDiskSample_() const
{
    const Vec3 sample = GenerateRandomVec3InDisk();
    return m_center + m_defocusDiskRight * sample.x + m_defocusDiskUp * sample.y;
}
