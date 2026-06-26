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
    void SetFOV(float _rad);
    void SetSample(Int32 _sample);
    void SetCenter(Vec3 _center);
    void SetLookTo(Vec3 _dir);
    void SetLookAt(Vec3 _point);
    void SetCameraUp(Vec3 _up);

    [[nodiscard]] Int32 GetWidth() const;
    [[nodiscard]] Int32 GetHeight() const;
    [[nodiscard]] float GetFOV() const;
    [[nodiscard]] Int32 GetSample() const;
    [[nodiscard]] Vec3  GetCenter() const;
    [[nodiscard]] Vec3  GetLookTo() const;

    void Render(const IHittable& _world);

    [[nodiscard]] const RGBImageBuffer& GetImageBuffer() const;

private:
    void UpdateDirty_();
    Vec3 RayColor_(const Ray& _ray, Int32 _depth, const IHittable& _world);

    constexpr static float kViewportHeight     = 2.f;
    constexpr static float kViewportHeightHalf = kViewportHeight * 0.5f;

    Int32 m_width  = 800;
    Int32 m_height = 600;

    Vec3  m_center   = Vec3::kZero;
    Vec3  m_lookTo   = Vec3::kUnitZ;
    Vec3  m_up       = Vec3::kUnitY;
    float m_fovRad   = ToRad(90.f);
    Int32 m_sample   = 100;   // 1픽셀당 샘플링 횟수
    Int32 m_maxDepth = 50;    // 최대 재귀 깊이

    // pre-calculated values
    float m_viewportWidth  = 0.f;
    Vec3  m_pixelDx        = Vec3::kZero;   // 1픽셀이 뷰포트에서 차지하는 크기 (카메라 로컬 x축)
    Vec3  m_pixelDy        = Vec3::kZero;   // 1픽셀이 뷰포트에서 차지하는 크기 (카메라 로컬 y축)
    Vec3  m_pixelStartPosW = Vec3::kZero;   // 뷰포트의 월드 공간 시작 위치

    RGBImageBuffer m_imageBuffer  = {};   // 렌더링 결과를 저장할 이미지 버퍼
    bool           m_bCameraDirty = true;
};
