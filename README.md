# Ray Tracing in One Weekend — C++ 구현

Peter Shirley의 [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html)을 학습용으로 직접 구현한 프로젝트다. 책을 참조하여 챕터에 맞게 레이 트레이서를 구현하고, 최종적으로 무작위 재질 구체 수백 개를 렌더링하는 장면을 완성한다.

---

## 결과

> 최종 씬: 500 samples/pixel · 1920×1080 · 피사계 심도 적용 · 렌더링 시간 10분 58.585초 (Release, OpenMP) 


---

## 구현 챕터

| 챕터 | 제목 | 핵심 내용 |
|------|------|-----------|
| 4 | Rays, Camera, Background | Ray 구조체, 기본 카메라, 하늘색 그라디언트 배경 |
| 5 | Adding a Sphere | 구-레이 교차 판정 (이차방정식 판별식) |
| 6 | Surface Normals & Multiple Objects | 노멀 시각화, `IHittable` 추상 인터페이스, `HittableList` |
| 7 | Camera Class | 카메라 로직 분리, Dirty Flag 패턴으로 재계산 최소화 |
| 8 | Antialiasing | 픽셀당 다중 샘플링 + 평균화 |
| 9 | Diffuse Materials | 람베르트 산란 (반구 균일 샘플 → 코사인 가중 샘플) |
| 10 | Metal | 완전 반사 + Fuzz 파라미터로 흐림 정도 제어 |
| 11 | Dielectric | 굴절(스넬의 법칙) + 슐릭 근사로 프레넬 반사 처리 |
| 12 | Positionable Camera | LookAt 기반 카메라, 임의 위치·방향·FOV 지원 |
| 13 | Defocus Blur | 렌즈 디스크 샘플링으로 피사계 심도(DoF) 구현 |
| 14 | Final Scene | 무작위 재질 구체 수백 개 + 세 가지 주인공 구체 |

---

## 구현 특이사항

### OpenMP 병렬 렌더링

픽셀 간 의존성이 없으므로 행 단위로 동적 스케줄링해 멀티코어를 활용한다.

```cpp
#pragma omp parallel for schedule(dynamic)
for (Int32 y = 0; y < m_height; ++y)
{
    for (Int32 x = 0; x < m_width; ++x)
    {
        // 픽셀당 독립적으로 ray cast
    }
}
```


### 재질 구조

```
Material  (추상 기반 클래스)
├── Lambertian   — 코사인 가중 산란 (난반사)
├── Metal        — 반사 벡터 + fuzz 교란 (금속 반사)
└── Dielectric   — 굴절(Snell) + 슐릭 프레넬 근사 (유리)
```


---

## 빌드 및 실행

### 요구사항

- C++20 이상 지원 컴파일러
- Windows 10 / 11
- Visual Studio 2022 (MSVC v143)
- OpenMP (MSVC 기본 포함)

### 종속성

- [stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h) — PNG 파일 저장용 헤더

### 실행

```
RayTracingInOneWeekend.exe
```

`RayTracingInOneWeekend/Outputs/` 디렉터리에 `14. Final Scene YYYY-MM-DD HHMMSS.png` 형식으로 결과가 저장된다.

### 파라미터 조정

`Main.cpp`의 카메라 설정을 수정해 렌더링 품질과 구도를 바꿀 수 있다.

```cpp
camera.SetSample(500);              // 픽셀당 샘플 수 (높을수록 선명하지만 느림)
camera.SetMaxDepth(50);             // 최대 반사·굴절 재귀 깊이
camera.SetFOV(ToRad(20.f));         // 수직 FOV
camera.SetDefocusRad(ToRad(0.6f));  // 렌즈 반각 (클수록 흐림 강함)
camera.SetFocusDistance(10.f);      // 초점 거리
```

