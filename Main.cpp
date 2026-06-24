#include "pch.h"
#include "Image.h"
#include "Timer.h"

constexpr std::string_view kOutputDir  = "Outputs";
constexpr std::string_view kSampleName = "4. Rays, a Simple Camera, and Background";

Vec3 RayColor(
    const Ray _ray)
{
    ASSERT(_ray.dir.IsNormalized(), "Ray direction must be normalized.");
    constexpr Vec3 kWhite { 1.f };
    constexpr Vec3 kSky = { 0.5f, 0.7f, 1.f };

    float a = 0.5f * (_ray.dir.y + 1.f);
    return kWhite.Lerp(kSky, a);
}

void SaveImageBuffer(
    const RGBImageBuffer& _image)
{
    const TimeStamp   ts       = TimeStamp::Current();
    const std::string filename = std::format("{} ({:04}.{:02}.{:02} {:02}h {:02}m {:02}s).png", kSampleName, ts.year, ts.month, ts.day, ts.hour, ts.min, ts.src);

    fs::path path = kOutputDir;
    path /= filename;
    _image.SaveToFile(path);
}

int main()
{
    // 이미지 spec
    constexpr float kDesireAspectRatio = 16.f / 9.f;
    constexpr float kHeight            = 256.f;
    constexpr float kWidth             = Floor(kHeight * kDesireAspectRatio);
    constexpr float kAspectRatio       = static_cast<float>(kWidth) / kHeight;

    // 뷰포트 spec
    constexpr float kViewportHeight     = 2.f;
    constexpr float kViewportWidth      = kViewportHeight * kAspectRatio;
    constexpr float kViewportHeightHalf = kViewportHeight / 2.f;
    constexpr float kViewportWidthHalf  = kViewportWidth / 2.f;
    constexpr float kFieldOfView        = ToRad(90.f);   // 시야각

    // 1픽셀이 뷰포트에서 차지하는 크기
    constexpr float kPixelW = kViewportWidth / kWidth;
    constexpr float kPixelH = kViewportHeight / kHeight;

    // 카메라 위치 계산
    const float focalLength = (kViewportHeight / 2.f) / ::tanf(kFieldOfView / 2.f);
    const Vec3  cmrPos      = { 0.f, 0.f, -focalLength };

    constexpr Int32 kiWidth  = static_cast<Int32>(kWidth);
    constexpr Int32 kiHeight = static_cast<Int32>(kHeight);
    RGBImageBuffer  image    = { kiWidth, kiHeight };
    for (Int32 y = 0; y < kiHeight; ++y)
    {
        for (Int32 x = 0; x < kiWidth; ++x)
        {
            // 스크린 좌표 -> 뷰포트 좌표
            const float yy  = kViewportHeightHalf - (static_cast<float>(y) + 0.5f) * kPixelH;
            const float xx  = (static_cast<float>(x) + 0.5f) * kPixelW - kViewportWidthHalf;
            const Vec3  dir = (Vec3 { xx, yy, 0.f } - cmrPos);
            const Ray   ray = { cmrPos, dir.Normalize() };
            image.WriteLinear(x, y, RayColor(ray));
        }
    }

    SaveImageBuffer(image);
    return 0;
}