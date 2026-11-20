<h1 align="center">Ray Tracing in One Weekend</h1>

<p align="center">C++로 만든 소프트웨어 레이 트레이서</p>

<p align="center">
  <img src="https://img.shields.io/badge/C++-00599C?logo=cplusplus&logoColor=white" alt="C++">
  <img src="https://img.shields.io/badge/OpenMP-parallel-5A5A5A" alt="OpenMP">
  <img src="https://img.shields.io/badge/Platform-Windows-0078D4" alt="Windows">
</p>

![확산·금속·유리 재질의 구와 디포커스 블러를 함께 적용한 최종 렌더링 결과](RayTracingInOneWeekend/Outputs/14.%20Final%20Scene%202026-06-27%20195327.png)

## Contents

* [프로젝트 소개](#프로젝트-소개)
* [교차와 재질의 분리](#교차와-재질의-분리)
* [병렬 렌더링](#병렬-렌더링)
* [빌드 및 실행](#빌드-및-실행)

## 프로젝트 소개

[Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)를 따라 구현한 CPU 레이 트레이서입니다. 카메라에서 픽셀마다 광선을 보내고, 광선이 구에 닿으면 재질에 따라 반사하거나 굴절한 광선을 다시 추적해 색을 모읍니다. 결과는 PNG로 저장합니다.

<p align="center">
  <img src="docs/images/diffuse.png" width="49%" alt="확산 반사를 적용한 회색 구와 바닥. 구 아래쪽에 어두운 음영이 생깁니다.">
  <img src="docs/images/metal.png" width="49%" alt="가운데 파란 확산 구와 주변 장면이 비치는 양옆의 금속 구">
</p>
<p align="center">
  <img src="docs/images/glass-camera.png" width="49%" alt="왼쪽부터 유리, 확산, 금속 재질의 구">
  <img src="docs/images/defocus.png" width="49%" alt="초점에서 벗어난 구의 윤곽이 흐려지는 디포커스 블러">
</p>

재질은 세 가지입니다. 확산 재질은 법선 쪽 반구로 광선을 무작위로 흩뿌리고, 금속은 거울 반사 방향에 거칠기(`fuzz`)만큼 흔들림을 더합니다. 유리는 Schlick 근사로 구한 반사율에 따라 반사와 굴절 중 하나를 고르고, 전반사 조건에서는 항상 반사합니다. 카메라는 위치, 시야각, 초점 거리, 디포커스 각도를 설정할 수 있으며, 렌즈 원판 위의 무작위 위치에서 광선을 출발시켜 초점 밖의 물체를 흐리게 합니다. 픽셀마다 여러 샘플을 평균내 안티에일리어싱과 잡음 감소를 함께 처리합니다.

최종 장면은 1920×1080, 픽셀당 500샘플, 최대 반사 깊이 50으로 렌더링했습니다. 22×22 격자에 반지름 0.2인 작은 구를 확산 80%, 금속 15%, 유리 5% 확률로 배치하고, 큰 구 세 개에 유리, 확산, 금속을 하나씩 적용했습니다.

## 교차와 재질의 분리

레이 트레이서는 광선마다 두 가지를 묻습니다. 무엇에 닿았는지, 그리고 닿은 표면이 빛을 어디로 보내는지입니다. 이 두 질문을 각각 인터페이스로 나눴습니다.

```cpp
class IHittable
{
public:
    virtual bool Hit(const Ray& _ray, Interval _interval, HitRecord& _outRecord) const = 0;
};

class Material
{
public:
    virtual bool Scatter(const Ray& _ray, const HitRecord& _record, Vec3& _attenuation, Ray& _scattered) const = 0;
};
```

`IHittable`은 교차 지점, 법선, 앞뒷면 여부와 함께 표면의 `Material` 포인터를 `HitRecord`에 담아 돌려줍니다. 색을 계산하는 카메라는 그 재질이 확산인지 금속인지 모른 채 `Scatter()`만 호출하고, 돌려받은 감쇠 색을 다음 광선의 색에 곱합니다.

```cpp
if (record.pMaterial->Scatter(_ray, record, attenuation, scattered))
{
    return attenuation * RayColor_(scattered, _depth + 1, _world);
}
return Vec3::kZero;
```

새 재질을 추가할 때는 `Material`을 상속해 `Scatter()`만 구현하면 되고, 카메라의 광선 추적 코드는 그대로입니다. 확산(`Lambertian`), 금속(`Metal`), 유리(`Dielectric`) 모두 이 구조입니다. 물체 목록인 `HittableList`도 `IHittable`을 구현하므로, 카메라는 구 하나와 장면 전체를 같은 방식으로 다룹니다. 목록은 교차를 찾을 때마다 탐색 범위의 최대 거리를 그 교차의 `t`로 줄여, 마지막에는 가장 가까운 표면만 남깁니다.

## 병렬 렌더링

픽셀은 서로 독립적으로 계산되므로 행 단위로 OpenMP 병렬화했습니다. 최종 장면은 픽셀 약 207만 개에 500샘플씩, 반사가 이어질 때마다 광선을 더 추적하므로 단일 스레드로는 오래 걸립니다.

```cpp
#pragma omp parallel for schedule(dynamic)
for (Int32 y = 0; y < m_height; ++y)
{
    for (Int32 x = 0; x < m_width; ++x)
    {
        ...
    }
}
```

행마다 계산량이 다릅니다. 하늘만 보이는 위쪽 행은 광선이 바로 배경에 닿고, 구가 몰린 아래쪽 행은 반사와 굴절이 여러 번 이어집니다. 행을 스레드 수로 똑같이 나누면 하늘 쪽을 맡은 스레드가 먼저 끝나고 쉬게 되므로, 일을 마친 스레드가 다음 행을 가져가는 `schedule(dynamic)`을 사용했습니다.

샘플링, 확산 방향, 렌즈 위치에는 모두 난수가 필요합니다. 난수 생성기 하나를 여러 스레드가 공유하면 상태를 동시에 바꾸므로 잠금이 필요하고, 잠금은 모든 광선이 거치는 경로에서 병목이 됩니다. 그래서 Marsaglia의 SHR3 생성기를 `thread_local`로 두어 스레드마다 독립된 상태를 갖게 했습니다. SHR3는 시프트와 XOR 세 번으로 다음 값을 만드는 생성기입니다.

## 빌드 및 실행

Windows에서 Visual Studio 2026(플랫폼 도구 집합 `v145`)로 빌드합니다. PNG 저장에 쓰는 `stb_image_write.h`는 저장소에 포함되어 있지 않으므로 별도로 받아 include 경로에 두어야 합니다.

1. 저장소를 받습니다.

   ```powershell
   git clone https://github.com/JuggernautSensei/RayTracingInOneWeekend.git
   ```

2. `RayTracingInOneWeekend.sln`을 Visual Studio로 열고 구성을 `Release | x64`로 둡니다.
3. 빌드한 뒤 실행합니다.

렌더링 중에는 콘솔에 진행률이 표시되고, 끝나면 `Outputs` 폴더에 장면 이름과 시각이 붙은 PNG가 저장됩니다. 이미지 크기, 샘플 수, 카메라 설정은 `Main.cpp`에서 바꿉니다.
