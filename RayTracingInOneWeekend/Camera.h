#pragma once
#include "Image.h"
#include "Random.h"

class IHittable;

class Camera
{
public:
    Camera() = default;
    Camera(Int32 _width, Int32 _height);

    void SetWidth(Int32 _width);
    void SetHeight(Int32 _height);
    void SetCenter(Vec3 _center);
    void SetFOV(float _fovRad);
    void SetSample(Int32 _sample);

    [[nodiscard]] Int32 GetWidth() const;
    [[nodiscard]] Int32 GetHeight() const;
    [[nodiscard]] Vec3  GetCenter() const;
    [[nodiscard]] float GetFOV() const;
    [[nodiscard]] Int32 GetSample() const;

    void Render(const IHittable& _world);

    [[nodiscard]] const RGBImageBuffer& GetImageBuffer() const;

private:
    void UpdatePaletteIfNeed_();
    Vec3 RayColor_(const Ray& _ray, Int32 _depth, const IHittable& _world);

    constexpr static float kViewportHeight     = 2.f;
    constexpr static float kViewportHeightHalf = kViewportHeight * 0.5f;

    Int32 m_width  = 800;
    Int32 m_height = 600;

    Vec3  m_center   = Vec3::kZero;
    float m_fovRad   = ToRad(90.f);
    Int32 m_sample   = 100;   // 1픽셀당 샘플링 횟수
    Int32 m_maxDepth = 50;   // 최대 재귀 깊이

    // palette
    float          m_viewportWidth = 0.f;
    float          m_pixelW        = 0.f;   // 1픽셀이 뷰포트에서 차지하는 크기
    float          m_pixelH        = 0.f;   // 1픽셀이 뷰포트에서 차지하는 크기
    float          m_focalLength   = 0.f;   // 초점 거리
    RGBImageBuffer m_imageBuffer   = {};    // 렌더링 결과를 저장할 이미지 버퍼
    bool           m_bPaletteDirty = true;
};
