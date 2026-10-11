<div align="center">

# Ray Tracing In One Weekend

**C++20으로 직접 구현한 CPU 레이 트레이서**

![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)
![Windows](https://img.shields.io/badge/Platform-Windows-0078D4?logo=windows&logoColor=white)
![MSVC](https://img.shields.io/badge/Compiler-MSVC%20v145-5C2D91?logo=visualstudio&logoColor=white)
![OpenMP](https://img.shields.io/badge/Parallel-OpenMP-blue)

[프로젝트 소개](#프로젝트-소개) · [결과](#결과) · [구현 챕터](#구현-챕터) · [코드 둘러보기](#코드-둘러보기) · [1. 카메라](#1-카메라) · [2. 재질](#2-재질과-다형성) · [3. 병렬 렌더링](#3-openmp로-병렬-렌더링) · [빌드 및 실행](#빌드-및-실행) · [회고](#회고)

</div>

<p align="center"><img src="RayTracingInOneWeekend/Outputs/14.%20Final%20Scene%202026-06-27%20195327.png" alt="최종 장면" width="840"></p>

<br>

## 프로젝트 소개

Peter Shirley의 [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html)를 따라 만든 레이 트레이서입니다. 카메라에서 픽셀마다 광선을 쏘고, 물체에 부딪힐 때마다 재질에 따라 산란시키며 색을 계산합니다. 외부 렌더링 라이브러리 없이 벡터, 난수 생성기, 카메라, 재질을 모두 직접 작성했고, 이미지 저장에만 `stb_image_write`를 사용합니다.

책의 코드를 그대로 옮기는 대신 C++20 스타일로 다시 설계했습니다. 충돌 가능한 객체는 `IHittable`, 재질은 `Material` 인터페이스 뒤로 숨겼고, 카메라는 설정값이 바뀔 때만 뷰포트를 다시 계산합니다. 1920×1080에서 픽셀당 500개 샘플을 쏘는 최종 장면은 OpenMP로 병렬화해 약 10분 58초에 렌더링됩니다.

<br>

## 결과

| 항목 | 값 |
|---|---|
| 장면 | 무작위 구체 약 480개 + 주인공 구체 3개 + 바닥 |
| 해상도 | 1920 × 1080 (16:9) |
| 샘플 수 | 픽셀당 500 |
| 최대 재귀 깊이 | 50 |
| 시야각 / 렌즈 | 수직 FOV 20° / 렌즈 각 0.6°, 초점 거리 10 |
| 렌더링 시간 | 약 10분 58초 (Release, OpenMP) |

<br>

## 구현 챕터

| 챕터 | 주제 | 핵심 내용 |
|:---:|---|---|
| 4 | Rays, Camera, Background | `Ray`, 하늘색 그라디언트 배경 |
| 5 | Sphere | 이차방정식 판별식으로 구-광선 교차 판정 |
| 6 | Normals & Multiple Objects | `IHittable` 인터페이스, `HittableList` |
| 7 | Camera | `Camera` 클래스 분리, Dirty Flag로 사전 계산값 갱신 |
| 8 | Antialiasing | 픽셀 안에서 무작위 위치를 여러 번 샘플링해 평균 |
| 9 | Diffuse | 람베르트 산란, 감마 보정 |
| 10 | Metal | 반사 + fuzz |
| 11 | Dielectric | 스넬 굴절, 전반사, 슐릭 근사 |
| 12 | Positionable Camera | `LookAt` · `CameraUp` · FOV |
| 13 | Defocus Blur | 렌즈 디스크 샘플링으로 피사계 심도 구현 |
| 14 | Final Scene | 무작위 재질 구체 수백 개 |

<br>

## 코드 둘러보기

### 파일 구성

```
RayTracingInOneWeekend
├── RayTracingInOneWeekend.sln
└── RayTracingInOneWeekend
    ├── Main.cpp                      장면 구성, 카메라 설정, 결과 저장
    ├── Camera.*                      뷰포트 계산, 광선 생성, 렌더 루프
    ├── IHittable.* HittableList.*    충돌 인터페이스와 객체 목록
    ├── Sphere.*                      구
    ├── Material.*                    재질 인터페이스
    │   ├── Lambertian.*              난반사
    │   ├── Metal.*                   금속 반사
    │   └── Dielectric.*              유리
    ├── Image.*                       RGB 버퍼와 PNG 저장
    ├── Vector.h  Math.h              Vec3와 수학 유틸
    ├── Random.*  RandomUtils.*       SHR3 · MWC · MT19937 난수 생성기
    └── Timer.*  Profile.*  Assertion.*   시간 측정, 프로파일러, ASSERT
```

### 광선 하나가 색이 되기까지

[`Camera::RayColor_()`](RayTracingInOneWeekend/Camera.cpp)는 광선이 어딘가에 부딪히면 재질에 산란을 맡기고, 산란된 광선의 색에 감쇠값을 곱하며 재귀합니다. 아무것에도 부딪히지 않으면 하늘색 그라디언트를 돌려줍니다.

```cpp
if (_world.Hit(_ray, interval, record))
{
    if (record.pMaterial->Scatter(_ray, record, attenuation, scattered))
    {
        return attenuation * RayColor_(scattered, _depth + 1, _world);
    }
    return Vec3::kZero;   // 흡수
}

const float a = 0.5f * (_ray.dir.y + 1.f);
return kWhite.Lerp(kSky, a);
```

충돌 검사 구간의 시작을 `0.001`로 둔 것은 반사된 광선이 자기 표면에 다시 부딪히는 셰도 애크니를 막기 위해서입니다.

<br>

## 1. 카메라

### 설정이 바뀔 때만 다시 계산합니다

뷰포트 크기, 픽셀 간격, 시작 위치는 렌더링 중에는 변하지 않는 값입니다. `Set...()` 함수가 `m_bCameraDirty`만 켜 두고, `Render()` 시작 시점의 `UpdateDirty_()`가 한 번에 계산합니다. 이미지 버퍼도 이때 만들어집니다.

```cpp
m_viewportHeight = 2 * ::tanf(m_fovRad * 0.5f) * m_focusDist;
m_viewportWidth  = m_viewportHeight * aspectRatio;

const Vec3 right = m_up.Cross(m_lookTo).Normalize();
const Vec3 up    = m_lookTo.Cross(right).Normalize();
```

뷰포트를 초점 거리 위에 놓으면 초점 평면에 있는 물체가 정확히 맞는 크기로 보이고, 피사계 심도 계산도 같은 평면을 기준으로 할 수 있습니다.

### 피사계 심도

렌즈를 지름이 `m_defocusRad`인 원판으로 보고, 광선의 시작점을 이 원판 위의 무작위 점으로 옮깁니다. 목표 지점은 그대로이므로 초점 평면 위의 물체는 선명하고 나머지는 번집니다. 렌즈 각이 0이면 원판 샘플링을 건너뛰고 카메라 중심에서 쏩니다.

```cpp
const Vec3 origin = IsZeroApprox(m_defocusRad) ? m_center : GenerateDefocusDiskSample_();
const Vec3 dir    = (pixel - origin).Normalize();
```

<br>

## 2. 재질과 다형성

`Material::Scatter()` 하나만 오버라이드하면 새 재질을 추가할 수 있습니다. 산란 여부와 감쇠값, 새 광선을 함께 돌려줍니다.

| 재질 | 산란 방식 |
|---|---|
| `Lambertian` | 법선에 단위 구 위의 무작위 벡터를 더한 방향. 결과가 영벡터에 가까우면 법선을 사용 |
| `Metal` | 반사 벡터에 fuzz 크기의 무작위 교란을 더함 |
| `Dielectric` | 굴절률 비로 굴절. 전반사 조건이거나 슐릭 근사 반사율이 무작위 값보다 크면 반사 |

유리는 광선이 물체 안에서 밖으로 나올 때 굴절률 비가 뒤집힙니다. `HitRecord`가 앞면 충돌 여부(`bFrontFace`)를 가지고 있어 `Dielectric`이 이를 보고 비율을 고릅니다.

```cpp
const float eta = _record.bFrontFace ? (1.f / m_ior) : m_ior;
const float cos = -_ray.dir.Dot(_record.normal);
const float sin = ::sqrtf(1.f - cos * cos);

if (eta * sin > 1.f || Reflectance(cos, eta) > GenerateRandomSHR3<float>(0.f, 1.f))
    dir = _ray.dir.Reflect(_record.normal).Normalize();   // 전반사 또는 프레넬 반사
else
    dir = _ray.dir.Refract(_record.normal, eta).Normalize();
```

재질은 `std::shared_ptr<Material>`로 들고 있어서 바닥이나 유리구처럼 여러 구체가 같은 재질을 공유할 수 있습니다.

<br>

## 3. OpenMP로 병렬 렌더링

픽셀끼리 의존성이 없어서 행 단위로 나누면 됩니다. 장면 오른쪽 아래처럼 광선이 오래 튕기는 영역과 하늘만 보이는 영역은 계산량이 크게 다르므로, 정적 분배 대신 `schedule(dynamic)`을 썼습니다.

```cpp
#pragma omp parallel for schedule(dynamic)
for (Int32 y = 0; y < m_height; ++y)
{
    for (Int32 x = 0; x < m_width; ++x)
    {
        Vec3 color = Vec3::kZero;
        for (int i = 0; i < m_sample; ++i)
            color += RayColor_(CalcRay_(x, y), 0, _world);

        color /= static_cast<float>(m_sample);
        m_imageBuffer.WriteLinear(x, y, GammaCorrect(color, 2.f));
    }
    // 10행마다 진행률 출력 (omp critical)
}
```

각 픽셀은 자기 칸에만 쓰기 때문에 이미지 버퍼에는 잠금이 필요 없습니다. 진행률 카운터는 `std::atomic`, 콘솔 출력은 `omp critical`로 보호합니다.

<br>

## 빌드 및 실행

Windows 10 또는 11과 Visual Studio(**C++를 사용한 데스크톱 개발** 워크로드, v145 툴셋, C++20, OpenMP)가 필요합니다. [`stb_image_write.h`](https://github.com/nothings/stb/blob/master/stb_image_write.h)는 저장소에 들어 있지 않으므로 include 경로에 직접 추가해 주세요.

1. Visual Studio에서 `RayTracingInOneWeekend.sln`을 엽니다.
2. 구성을 `Release`, 플랫폼을 `x64`로 바꿉니다.
3. **Ctrl + F5**로 실행하면 `Outputs/` 폴더에 `14. Final Scene YYYY-MM-DD HHMMSS.png`가 저장됩니다.

> [!TIP]
> 처음에는 `Main.cpp`의 `camera.SetSample()`을 10~50 정도로 낮춰 구도만 확인하세요. 샘플 수에 비례해 렌더링 시간이 늘어납니다.

### 파라미터

```cpp
camera.SetSample(500);              // 픽셀당 샘플 수
camera.SetMaxDepth(50);             // 최대 재귀 깊이
camera.SetFOV(ToRad(20.f));         // 수직 FOV
camera.SetCenter({ 13.f, 2.f, -3.f });
camera.SetLookAt({ 0.f, 0.f, 0.f });
camera.SetDefocusRad(ToRad(0.6f));  // 클수록 배경이 더 흐려짐
camera.SetFocusDistance(10.f);      // 초점 거리
```

Debug 빌드에서는 `FUNCTION_PROFILE()`이 활성화되어 함수별 실행 시간이 출력되고 `ASSERT`가 동작합니다.
