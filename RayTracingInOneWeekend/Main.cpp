#include "pch.h"

#include "Camera.h"
#include "Dielectric.h"
#include "HittableList.h"
#include "Image.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Sphere.h"

constexpr std::string_view kOutputDir  = "Outputs";
constexpr std::string_view kSampleName = "11. Dielectric";

void SaveImageBuffer(
    const RGBImageBuffer& _image)
{
    FUNCTION_PROFILE();

    const TimeStamp   ts       = TimeStamp::Current();
    const std::string filename = std::format("{} {:04}-{:02}-{:02} {:02}{:02}{:02}.png", kSampleName, ts.year, ts.month, ts.day, ts.hour, ts.min, ts.src);

    fs::path path = kOutputDir;
    path /= filename;
    _image.SaveToPngFile(path);
}

int main()
{
    constexpr float kDesireAspectRatio = 16.f / 9.f;
    constexpr Int32 kHeight            = 1080;
    constexpr Int32 kWidth             = static_cast<Int32>(kHeight * kDesireAspectRatio);

    // world
    const std::shared_ptr<Material> pGroundMat = std::make_shared<Lambertian>(Vec3 { 0.8f, 0.8f, 0.f });
    const std::shared_ptr<Material> pCenterMat = std::make_shared<Lambertian>(Vec3 { 0.1f, 0.2f, 0.5f });
    const std::shared_ptr<Material> pLeftMat   = std::make_shared<Dielectric>(1.5f);
    const std::shared_ptr<Material> pBubbleMat = std::make_shared<Dielectric>(1.f / 1.5f);
    const std::shared_ptr<Material> pRightMat  = std::make_shared<Metal>(Vec3 { 0.8f, 0.6f, 0.2f }, 0.3f);

    HittableList world = {};
    world.Add(std::make_unique<Sphere>(Vec3 { 0.f, -100.5f, 1.f }, 100.f, pGroundMat));
    world.Add(std::make_unique<Sphere>(Vec3 { 0.f, 0.f, 1.2f }, 0.5f, pCenterMat));
    world.Add(std::make_unique<Sphere>(Vec3 { -1.f, 0.f, 1.f }, 0.5f, pLeftMat));
    world.Add(std::make_unique<Sphere>(Vec3 { -1.f, 0.f, 1.f }, 0.4f, pBubbleMat));
    world.Add(std::make_unique<Sphere>(Vec3 { 1.f, 0.f, 1.f }, 0.5f, pRightMat));

    // camera
    Camera camera = { kWidth, kHeight };
    camera.SetFOV(ToRad(90.f));
    camera.Render(world);
    SaveImageBuffer(camera.GetImageBuffer());
    return 0;
}