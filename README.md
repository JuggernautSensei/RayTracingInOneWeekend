<div align="center">

# Ray Tracing in One Weekend

**C++ 소프트웨어 레이 트레이서**

![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)
![Windows](https://img.shields.io/badge/Platform-Windows-0078D4)
![OpenMP](https://img.shields.io/badge/OpenMP-Parallel-4D4D4D)

<p align="center"><img src="RayTracingInOneWeekend/Outputs/14.%20Final%20Scene%202026-06-27%20195327.png" alt="회색 바닥 위에 색색의 작은 구와 유리·확산·금속 재질의 큰 구 세 개가 놓인 최종 씬" width="840"></p>

*1920×1080, 픽셀당 500샘플, 최대 반사 깊이 50으로 렌더링한 최종 씬*

</div>

## Contents

- [프로젝트 소개](#프로젝트-소개)
- [광선과 교차](#광선과-교차)
- [재질](#재질)
- [카메라](#카메라)
- [최종 씬](#최종-씬)
- [코드 구조](#코드-구조)

<br>

## 프로젝트 소개

[Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)를 따라 만든 C++ 레이 트레이서입니다. 책에 실린 코드는 사용하지 않았습니다. 각 장의 설명만 읽고 클래스 구성과 구현 코드는 모두 직접 설계하고 작성했습니다.

구현한 기능은 다음과 같습니다.

- 카메라에서 픽셀마다 광선을 쏘고, 아무것도 맞지 않으면 하늘 배경색을 반환
- 광선과 구의 교차 판정, 표면 법선과 앞면/뒷면 구분
- 픽셀 내 무작위 샘플링을 이용한 안티에일리어싱
- 확산(Lambertian), 금속(Metal), 유리(Dielectric) 재질과 감마 보정
- 시야각, 위치, 바라보는 지점을 지정할 수 있는 카메라와 디포커스 블러
- 렌더링 결과 PNG 저장

<br>

## 광선과 교차

### 광선과 배경

광선은 시작점과 방향으로 정의합니다. 카메라는 뷰포트의 각 픽셀을 향해 광선을 쏘고, 광선이 아무것도 맞지 않으면 방향의 y 성분에 따라 흰색과 하늘색을 보간해 배경을 그립니다.

```cpp
constexpr Vec3 kWhite { 1.f };
constexpr Vec3 kSky = { 0.5f, 0.7f, 1.f };
const float    a    = 0.5f * (_ray.dir.y + 1.f);
return kWhite.Lerp(kSky, a);
```

### 구 교차

광선 위의 점이 구 표면에 놓이는 조건을 `t`에 대한 이차방정식으로 정리하고, 판별식이 음수이면 교차하지 않는다고 판단합니다. 근이 있으면 가까운 근부터 유효 범위 안에 있는지 확인하고, 벗어나면 먼 근을 확인합니다.

```cpp
const Vec3  u  = _ray.origin - m_center;
const float vu = _ray.dir.Dot(u);
const float a  = _ray.dir.LengthSq();
const float c  = u.LengthSq() - m_radius * m_radius;
const float d  = vu * vu - a * c;   // 판별식
```

### 여러 오브젝트와 앞뒷면

교차 지점의 법선이 광선과 같은 쪽을 향하면 광선이 구 안쪽에서 나가는 경우로 보고, 법선을 뒤집어 `bFrontFace`를 `false`로 기록합니다. 여러 오브젝트를 순회할 때는 교차할 때마다 탐색 범위의 최댓값을 줄여 가장 가까운 교차만 남깁니다.

```cpp
for (const std::unique_ptr<IHittable>& pHittable: m_hittables)
{
    if (pHittable->Hit(_ray, _interval, _outRecord))
    {
        bAnyHit       = true;
        _interval.max = _outRecord.t;   // 가장 가까운 hit만 기록
    }
}
```

<br>

## 재질

교차가 일어나면 재질의 `Scatter`가 감쇠 색과 산란된 광선을 정하고, `Camera::RayColor_`가 산란 광선을 최대 깊이까지 재귀적으로 추적합니다. 자기 자신과 다시 교차하는 문제를 피하려고 `t`의 최솟값은 0.001로 둡니다.

### 확산

법선에 무작위 단위 벡터를 더한 방향으로 광선을 산란시켜 Lambertian 반사를 표현합니다. 더한 결과가 0에 가까우면 법선 방향을 사용합니다.

```cpp
const Vec3 sample = GenerateRandomUnitVecOnSphere();
Vec3       dir    = _record.normal + sample;
if (dir.IsZeroApprox())
{
    _scattered = { _record.point, _record.normal };
}
```

### 금속

입사 광선을 법선 기준으로 반사시키고, fuzz 크기만큼 무작위 벡터를 더해 거친 표면을 표현합니다. 반사 방향이 표면 안쪽을 향하면 광선을 흡수합니다.

```cpp
const Vec3 reflected = (_ray.dir.Reflect(_record.normal) + GenerateRandomUnitVecOnSphere() * m_fuzz).Normalize();
_scattered           = { _record.point, reflected };
_attenuation         = m_albedo;
return reflected.Dot(_record.normal) > 0.f;
```

### 유리

굴절률 비율로 스넬의 법칙에 따른 굴절 방향을 계산합니다. 굴절이 불가능한 전반사 조건이거나, Schlick 근사로 구한 반사율이 난수보다 크면 반사시킵니다.

```cpp
const float eta = _record.bFrontFace ? (1.f / m_ior) : m_ior;
const float cos = -_ray.dir.Dot(_record.normal);
const float sin = ::sqrtf(1.f - cos * cos);

if (eta * sin > 1.f || Reflectance(cos, eta) > GenerateRandomSHR3<float>(0.f, 1.f))
```

### 감마 보정

샘플을 평균낸 선형 색에 감마 2의 보정을 적용한 뒤 이미지 버퍼에 기록합니다.

```cpp
color /= static_cast<float>(m_sample);
m_imageBuffer.WriteLinear(x, y, GammaCorrect(color, 2.f));
```

<br>

## 카메라

### 안티에일리어싱

픽셀 하나에 여러 광선을 쏘되, 광선마다 픽셀 안의 위치를 무작위로 흔들고 결과 색을 평균내 경계의 계단 현상을 줄입니다. 샘플이 1개일 때는 픽셀 중앙을 사용합니다.

```cpp
const float xOffset = m_sample == 1 ? 0.5f : GenerateRandomSHR3<float>(0.f, 1.f);
const float yOffset = m_sample == 1 ? 0.5f : GenerateRandomSHR3<float>(0.f, 1.f);
```

### 위치 지정

수직 시야각과 초점 거리로 뷰포트 크기를 정하고, 바라보는 방향과 위쪽 벡터로 카메라의 오른쪽·위쪽 기저를 계산합니다. setter는 값이 바뀔 때만 dirty 플래그를 세우고, 이 계산은 렌더링 직전에 `UpdateDirty_`에서 한 번만 수행합니다.

```cpp
m_viewportHeight        = 2 * ::tanf(m_fovRad * 0.5f) * m_focusDist;
m_viewportWidth         = m_viewportHeight * aspectRatio;

const Vec3 right = m_up.Cross(m_lookTo).Normalize();
const Vec3 up    = m_lookTo.Cross(right).Normalize();
```

### 디포커스 블러

광선의 시작점을 카메라 중심 대신 렌즈 원판 위의 무작위 점으로 바꿔 초점면에서 벗어난 물체가 흐려지게 합니다. 렌즈 반지름은 디포커스 각도와 초점 거리로 정합니다.

```cpp
const Vec3 sample = GenerateRandomVec3InDisk();
return m_center + m_defocusDiskRight * sample.x + m_defocusDiskUp * sample.y;
```

<br>

## 최종 씬

격자 위에 반지름 0.2의 작은 구를 무작위로 배치하고, 확산 80%, 금속 15%, 유리 5%의 비율로 재질을 정했습니다. 가운데에는 유리, 확산, 금속 재질의 큰 구 세 개를 두었습니다.

```cpp
if (materialSelector < 0.8f)
{
    // diffuse
    ...
}
else if (materialSelector < 0.95f)
{
    // metal
    ...
}
else
{
    // glass
    pMaterial = std::make_shared<Dielectric>(1.5f);
}
```

카메라는 시야각 20°, 디포커스 각도 0.6°, 초점 거리 10으로 설정했습니다.

<br>

## 코드 구조

| 파일 | 역할 |
|---|---|
| `Camera` | 뷰포트와 카메라 기저 계산, 픽셀별 광선 생성, 광선 추적과 렌더링 |
| `IHittable`, `HittableList`, `Sphere` | 광선과 교차할 수 있는 오브젝트의 인터페이스, 오브젝트 목록, 구 |
| `Material`, `Lambertian`, `Metal`, `Dielectric` | 재질 인터페이스와 확산·금속·유리 재질의 산란 계산 |
| `Vector`, `Ray`, `Interval` | 벡터 연산(반사·굴절 포함), 광선, 유효한 `t` 범위 |
| `Random`, `RandomUtils`, `VectorUtils` | 난수 생성기, 단위 구·원판 위의 무작위 벡터, 감마 보정 |
| `Image` | RGB 이미지 버퍼와 PNG 저장 |
| `Profile`, `Timer`, `Assertion` | 함수 단위 시간 측정, 타이머, 어서션 |
| `Main` | 최종 씬 구성과 카메라 설정 |
