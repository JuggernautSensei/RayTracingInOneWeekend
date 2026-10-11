# Ray Tracing in One Weekend (C++20)

Peter Shirley의 [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html)를 따라 C++20으로 직접 구현한 CPU 레이 트레이서입니다. 책의 최종 장면(무작위 구체 약 480개)을 렌더링하고 PNG로 저장합니다.

## 결과

![Final Scene](RayTracingInOneWeekend/Outputs/14.%20Final%20Scene%202026-06-27%20195327.png)

| 항목 | 값 |
|------|----|
| 해상도 | 1920 × 1080 (16:9) |
| 샘플 수 | 500 samples / pixel |
| 최대 재귀 깊이 | 50 |
| 렌더링 시간 | 약 10분 58초 (Release, OpenMP) |

## 구현 내용

| 챕터 | 주제 | 요약 |
|------|------|------|
| 4 | Rays, Camera, Background | `Ray`, 하늘색 그라디언트 배경 |
| 5 | Sphere | 이차방정식 판별식 기반 구-레이 교차 |
| 6 | Normals & Multiple Objects | `IHittable` 인터페이스, `HittableList` |
| 7 | Camera | `Camera` 클래스 분리, Dirty Flag로 사전 계산값 갱신 |
| 8 | Antialiasing | 픽셀당 다중 샘플링 후 평균 |
| 9 | Diffuse | `Lambertian` 산란, 감마 보정 |
| 10 | Metal | 반사 + fuzz |
| 11 | Dielectric | 스넬 굴절, 전반사, 슐릭 근사 |
| 12 | Positionable Camera | `LookAt` / `CameraUp` / FOV |
| 13 | Defocus Blur | 렌즈 디스크 샘플링(피사계 심도) |
| 14 | Final Scene | 무작위 재질 구체 + 주인공 구체 3개 |

## 구조

```
Main.cpp              장면 구성, 카메라 설정, 결과 저장
Camera.*              뷰포트 계산, 레이 생성, 렌더 루프(OpenMP)
IHittable.*           충돌 가능 객체 인터페이스
HittableList.*        객체 목록
Sphere.*              구
Material.*            재질 기반 클래스
 ├─ Lambertian.*      난반사
 ├─ Metal.*           금속 반사 (fuzz)
 └─ Dielectric.*      유리 (굴절/반사)
Image.*               RGB 버퍼, PNG 저장 (stb_image_write)
Vector.h / Math.h     Vec3 및 수학 유틸
Random.* / RandomUtils.*  SHR3 / MWC / MT19937 난수 생성기
Timer.* / Profile.*   시간 측정 및 스코프 프로파일러
Assertion.*           ASSERT 매크로
```

## 특징

- **OpenMP 병렬 렌더링**: 행 단위 `schedule(dynamic)`으로 분배하고, 진행률을 콘솔에 출력합니다.
- **난수 생성기**: 빠른 SHR3, MWC, 고품질 MT19937을 `RandomGeneratorT` 컨셉으로 교체 가능하게 구성했습니다.
- **프로파일링**: `FUNCTION_PROFILE()` 매크로로 스코프별 실행 시간을 측정합니다(Debug 빌드에서 활성화).

## 빌드 및 실행

### 요구사항

- Windows 10 / 11
- Visual Studio (플랫폼 툴셋 v145, C++20, OpenMP 지원)
- [`stb_image_write.h`](https://github.com/nothings/stb/blob/master/stb_image_write.h) — 저장소에 포함되어 있지 않으므로 컴파일러 include 경로에 추가해야 합니다.

### 방법

1. `RayTracingInOneWeekend.sln`을 Visual Studio로 엽니다.
2. `Release | x64`로 빌드하고 실행합니다.
3. 결과는 `Outputs/<샘플명> YYYY-MM-DD HHMMSS.png`로 저장됩니다.

### 파라미터 조정 (`Main.cpp`)

```cpp
camera.SetSample(500);              // 픽셀당 샘플 수
camera.SetMaxDepth(50);             // 최대 재귀 깊이
camera.SetFOV(ToRad(20.f));         // 수직 FOV
camera.SetCenter({ 13.f, 2.f, -3.f });
camera.SetLookAt({ 0.f, 0.f, 0.f });
camera.SetDefocusRad(ToRad(0.6f));  // 렌즈 반각 (클수록 흐림)
camera.SetFocusDistance(10.f);      // 초점 거리
```

빠르게 확인하려면 `SetSample`을 10~50 정도로 낮추세요.

## 참고

- [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
