# Ray Tracing in One Weekend

![최종 씬](RayTracingInOneWeekend/Outputs/14.%20Final%20Scene%202026-06-27%20195327.png)

*최종 씬 렌더 결과입니다. 1920×1080 해상도, 픽셀당 500샘플, 최대 반사 깊이 50으로 렌더링했습니다.*

[Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)를 따라 만든 C++ 레이 트레이서입니다. 책에 실린 코드는 사용하지 않았습니다. 각 장의 설명만 읽고 클래스 구성과 구현 코드는 모두 직접 설계하고 작성했습니다.

## Contents

- [구현 범위](#구현-범위)
- [코드 구조](#코드-구조)
- [챕터별 학습](#챕터별-학습)

## 구현 범위

- 카메라에서 픽셀마다 광선을 쏘고, 아무것도 맞지 않으면 하늘 배경색을 반환
- 광선과 구의 교차 판정
- 표면 법선 계산, 앞면/뒷면 구분, 여러 오브젝트 중 가장 가까운 교차 선택
- 픽셀 내 무작위 샘플링을 이용한 안티에일리어싱
- 확산(Lambertian), 금속(Metal), 유리(Dielectric) 재질
- 감마 보정
- 시야각, 위치, 바라보는 지점을 지정할 수 있는 카메라
- 디포커스 블러(피사계 심도)
- 렌더링 결과 PNG 저장

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

## 챕터별 학습

### 이미지 출력과 벡터 (2~3장)

렌더링 결과를 픽셀 단위로 기록할 이미지와, 위치·방향·색을 함께 표현할 3차원 벡터를 먼저 만들었습니다. 책은 PPM 형식으로 출력하지만, 이 프로젝트는 stb_image_write로 PNG 파일을 저장합니다.

- `RGBImageBuffer::WriteLinear`, `RGBImageBuffer::SaveToPngFile`
- `Vec3`

### 광선, 카메라, 배경 (4장)

광선을 시작점과 방향으로 정의하고, 카메라 위치에서 뷰포트의 각 픽셀을 향해 광선을 쏘았습니다. 광선이 아무것도 맞지 않으면 방향의 y 성분에 따라 흰색과 하늘색을 보간해 배경을 그립니다.

- `Ray`
- `Camera::RayColor_`의 배경 보간

### 구 추가 (5장)

광선 위의 점이 구 표면에 놓이는 조건을 `t`에 대한 이차방정식으로 정리하고, 판별식으로 교차 여부를 판단했습니다.

- `Sphere::Hit`

### 표면 법선과 여러 오브젝트 (6장)

교차 지점의 법선을 구하고, 광선이 구의 바깥에서 들어왔는지 안에서 나가는지를 구분해 법선이 항상 광선과 반대쪽을 향하도록 했습니다. 여러 오브젝트를 순회할 때는 교차할 때마다 탐색 범위의 최댓값을 줄여 가장 가까운 교차만 남깁니다.

- `HitRecord`의 `bFrontFace`
- `HittableList::Hit`
- `Interval`

### 카메라 클래스 분리 (7장)

뷰포트 계산과 렌더링 루프를 `Camera` 클래스로 옮겼습니다. 설정값은 setter로 바꾸며, 값이 바뀌면 dirty 플래그를 세우고 렌더링 직전에 뷰포트 관련 값을 한 번만 다시 계산합니다.

- `Camera`의 setter
- `Camera::UpdateDirty_`, `Camera::Render`

### 안티에일리어싱 (8장)

픽셀 하나에 여러 광선을 쏘되, 광선마다 픽셀 안의 위치를 무작위로 흔들고 결과 색을 평균내 경계의 계단 현상을 줄였습니다. 샘플이 1개일 때는 픽셀 중앙을 사용합니다.

- `Camera::CalcRay_`
- `Camera::Render`의 샘플 평균

### 확산 재질 (9장)

교차 지점에서 법선에 무작위 단위 벡터를 더한 방향으로 광선을 다시 산란시켜 Lambertian 반사를 구현했습니다. 산란 방향이 0에 가까우면 법선 방향을 사용합니다. 자기 자신과 다시 교차하는 문제를 피하려고 `t`의 최솟값을 0.001로 두었고, 결과 색에는 감마 2의 보정을 적용합니다.

- `Lambertian::Scatter`
- `GenerateRandomUnitVecOnSphere`, `GammaCorrect`

### 금속 (10장)

입사 광선을 법선 기준으로 반사시키고, 반사 방향에 fuzz 크기만큼 무작위 벡터를 더해 거친 금속을 표현했습니다. 반사 방향이 표면 안쪽을 향하면 광선을 흡수합니다.

- `Metal::Scatter`
- `Vec3::Reflect`

### 유리 (11장)

굴절률 비율로 스넬의 법칙에 따른 굴절 방향을 계산했습니다. 굴절이 불가능한 전반사 조건이거나, Schlick 근사로 구한 반사율이 난수보다 크면 반사시킵니다. 이 장의 구현은 12장 커밋에 함께 포함되어 있습니다.

- `Dielectric::Scatter`
- `Vec3::Refract`

### 위치 지정 카메라 (12장)

수직 시야각으로 뷰포트 크기를 정하고, 카메라 위치·바라보는 지점·위쪽 벡터로 카메라의 오른쪽·위쪽 기저를 계산해 원하는 위치와 방향에서 장면을 볼 수 있게 했습니다.

- `Camera::SetFOV`, `Camera::SetCenter`, `Camera::SetLookAt`, `Camera::SetCameraUp`
- `Camera::UpdateDirty_`

### 디포커스 블러 (13장)

광선의 시작점을 카메라 중심 대신 렌즈 원판 위의 무작위 점으로 바꾸고, 뷰포트를 초점 거리에 두어 초점면에서 벗어난 물체가 흐려지게 했습니다. 렌즈 크기는 디포커스 각도와 초점 거리로 정합니다.

- `Camera::GenerateDefocusDiskSample_`, `GenerateRandomVec3InDisk`
- `Camera::SetDefocusRad`, `Camera::SetFocusDistance`

### 최종 씬 (14장)

격자 위에 작은 구를 무작위로 배치하고, 확산 80%, 금속 15%, 유리 5%의 비율로 재질을 정했습니다. 가운데에는 유리, 확산, 금속 재질의 큰 구 세 개를 두었습니다. 카메라는 시야각 20°, 디포커스 각도 0.6°, 초점 거리 10으로 설정했습니다.

- `main`
