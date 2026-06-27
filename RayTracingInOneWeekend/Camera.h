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
    void SetMaxDepth(Int32 _depth);
    void SetCenter(Vec3 _center);
    void SetLookTo(Vec3 _dir);
    void SetLookAt(Vec3 _point);
    void SetCameraUp(Vec3 _up);
    void SetDefocusRad(float _defocusRad);
    void SetFocusDistance(float _dist);

    [[nodiscard]] Int32 GetWidth() const;
    [[nodiscard]] Int32 GetHeight() const;
    [[nodiscard]] float GetFOV() const;
    [[nodiscard]] Int32 GetSample() const;
    [[nodiscard]] Int32 GetMaxDepth() const;
    [[nodiscard]] Vec3  GetCenter() const;
    [[nodiscard]] Vec3  GetLookTo() const;
    [[nodiscard]] Vec3  GetCameraUp() const;
    [[nodiscard]] float GetDefocusRad() const;
    [[nodiscard]] float GetFocusDistance() const;

    void Render(const IHittable& _world);

    [[nodiscard]] const RGBImageBuffer& GetImageBuffer() const;

private:
    void               UpdateDirty_();
    [[nodiscard]] Vec3 RayColor_(const Ray& _ray, Int32 _depth, const IHittable& _world);
    [[nodiscard]] Ray  CalcRay_(Int32 _xScreen, Int32 _yScreen) const;
    [[nodiscard]] Vec3 GenerateDefocusDiskSample_() const;

    Int32 m_width  = 800;
    Int32 m_height = 600;

    Vec3  m_center   = Vec3::kZero;
    Vec3  m_lookTo   = Vec3::kUnitZ;
    Vec3  m_up       = Vec3::kUnitY;
    float m_fovRad   = ToRad(90.f);
    Int32 m_sample   = 100;   // 1픽셀당 샘플링 횟수
    Int32 m_maxDepth = 50;    // 최대 재귀 깊이

    // defocus
    float m_defocusRad = 0.f;   // defocus blur를 위한 렌즈 반지름
    float m_focusDist  = 1.f;   // defocus blur를 위한 초점 거리

    // pre-calculated values
    float m_viewportHeight   = 0.f;
    float m_viewportWidth    = 0.f;
    Vec3  m_pixelDeltaRight  = Vec3::kZero;   // 1픽셀이 뷰포트에서 차지하는 크기
    Vec3  m_pixelDeltaUp     = Vec3::kZero;   // 1픽셀이 뷰포트에서 차지하는 크기
    Vec3  m_pixelStartPosW   = Vec3::kZero;   // 뷰포트의 월드 공간 시작 위치
    Vec3  m_defocusDiskRight = Vec3::kZero;   // defocus blur를 위한 렌즈 로컬 x축
    Vec3  m_defocusDiskUp    = Vec3::kZero;   // defocus blur를 위한 렌즈 로컬 y축

    RGBImageBuffer m_imageBuffer  = {};   // 렌더링 결과를 저장할 이미지 버퍼
    bool           m_bCameraDirty = true;
};
