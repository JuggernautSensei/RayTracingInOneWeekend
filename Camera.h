#pragma once
#include "Image.h"

class IHittable;

class Camera
{
public:
    Camera() = default;
    Camera(Int32 _width, Int32 _height, Vec3 _center = Vec3::kZero, float _fovRad = ToRad(90.f));

    void SetWidth(Int32 _width);
    void SetHeight(Int32 _height);
    void SetCenter(Vec3 _center);
    void SetFOV(float _fovRad);

    [[nodiscard]] Int32 GetWidth() const;
    [[nodiscard]] Int32 GetHeight() const;
    [[nodiscard]] Vec3  GetCenter() const;
    [[nodiscard]] float GetFOV() const;

    void Render(const IHittable& _world);

    [[nodiscard]] const RGBImageBuffer& GetImageBuffer() const;

private:
    void UpdatePaletteIfNeed_();
    Vec3 RayColor_(const Ray& _ray, const IHittable& _world) const;

    constexpr static float kViewportHeight     = 2.f;
    constexpr static float kViewportHeightHalf = kViewportHeight * 0.5f;

    Int32 m_width  = 800;
    Int32 m_height = 600;

    Vec3  m_center = Vec3::kZero;
    float m_fovRad = ToRad(90.f);

    // palette
    float          m_viewportWidth = 0.f;
    float          m_pixelW        = 0.f;   // 1픽셀이 뷰포트에서 차지하는 크기
    float          m_pixelH        = 0.f;   // 1픽셀이 뷰포트에서 차지하는 크기
    float          m_focalLength   = 0.f;   // 초점 거리
    RGBImageBuffer m_imageBuffer   = {};    // 렌더링 결과를 저장할 이미지 버퍼
    bool           m_bPaletteDirty = true;
};
