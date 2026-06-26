#include "pch.h"

#include "Camera.h"
#include "HittableList.h"
#include "Image.h"
#include "Sphere.h"


constexpr std::string_view kOutputDir  = "Outputs";
constexpr std::string_view kSampleName = "9. Diffuse Materials";

void SaveImageBuffer(
    const RGBImageBuffer& _image)
{
    FUNCTION_PROFILE();

    const TimeStamp   ts       = TimeStamp::Current();
    const std::string filename = std::format("{} {:04}-{:02}-{:02} {:02}{:02}{:02}.png", kSampleName, ts.year, ts.month, ts.day, ts.hour, ts.min, ts.src);

    fs::path path = kOutputDir; 
    path /= filename;
    _image.SaveToFile(path);
}

int main()
{
    constexpr float kDesireAspectRatio = 16.f / 9.f;
    constexpr Int32 kHeight            = 512;
    constexpr Int32 kWidth             = static_cast<Int32>(kHeight * kDesireAspectRatio);

    // world
    HittableList world = {};
    world.Add(std::make_unique<Sphere>(Vec3 { 0.f, 0.f, 1.f }, 0.5f));
    world.Add(std::make_unique<Sphere>(Vec3 { 0.f, -100.5f, 1.f }, 100.f));

    // camera
    Camera camera = { kWidth, kHeight };
    camera.Render(world);
    SaveImageBuffer(camera.GetImageBuffer());
    return 0;
}