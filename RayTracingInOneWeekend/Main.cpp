#include "pch.h"

#include "Camera.h"
#include "Dielectric.h"
#include "HittableList.h"
#include "Image.h"
#include "Lambertian.h"
#include "Metal.h"
#include "RandomUtils.h"
#include "Sphere.h"

constexpr std::string_view kOutputDir  = "Outputs";
constexpr std::string_view kSampleName = "14. Final Scene";

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

    HittableList                world           = {};
    std::shared_ptr<Lambertian> pGroundMaterial = std::make_shared<Lambertian>(Vec3 { 0.5f, 0.5f, 0.5f });
    world.Add(std::make_unique<Sphere>(Vec3 { 0.f, -1000.f, 0.f }, 1000.f, pGroundMaterial));

    for (Int32 i = -11; i < 11; ++i)
    {
        for (Int32 j = -11; j < 11; ++j)
        {
            const float materialSelector = GenerateRandomSHR3<float>(0.f, 1.f);
            const Vec3  center           = {
                static_cast<float>(i) + 0.9f * GenerateRandomSHR3<float>(0.f, 1.f),
                0.2f,
                static_cast<float>(j) + 0.9f * GenerateRandomSHR3<float>(0.f, 1.f)
            };

            if ((center - Vec3 { 4.f, 0.2f, 0.f }).Length() > 0.9f)
            {
                std::shared_ptr<Material> pMaterial = nullptr;
                if (materialSelector < 0.8f)
                {
                    // diffuse
                    const Vec3 albedo = { GenerateRandomSHR3<float>(0.f, 1.f) * GenerateRandomSHR3<float>(0.f, 1.f), GenerateRandomSHR3<float>(0.f, 1.f) * GenerateRandomSHR3<float>(0.f, 1.f), GenerateRandomSHR3<float>(0.f, 1.f) * GenerateRandomSHR3<float>(0.f, 1.f) };
                    pMaterial         = std::make_shared<Lambertian>(albedo);
                }
                else if (materialSelector < 0.95f)
                {
                    // metal
                    const Vec3  albedo = { GenerateRandomSHR3<float>(0.5f, 1.f), GenerateRandomSHR3<float>(0.5f, 1.f), GenerateRandomSHR3<float>(0.5f, 1.f) };
                    const float fuzz   = GenerateRandomSHR3<float>(0.f, 0.5f);
                    pMaterial          = std::make_shared<Metal>(albedo, fuzz);
                }
                else
                {
                    // glass
                    pMaterial = std::make_shared<Dielectric>(1.5f);
                }

                world.Add(std::make_unique<Sphere>(center, 0.2f, pMaterial));
            }
        }
    }

    const std::shared_ptr<Dielectric> pMaterial1 = std::make_shared<Dielectric>(1.5f);
    world.Add(std::make_unique<Sphere>(Vec3 { 0.f, 1.f, 0.f }, 1.f, pMaterial1));

    const std::shared_ptr<Lambertian> pMaterial2 = std::make_shared<Lambertian>(Vec3 { 0.4f, 0.2f, 0.1f });
    world.Add(std::make_unique<Sphere>(Vec3 { -4.f, 1.f, 0.f }, 1.f, pMaterial2));

    const std::shared_ptr<Metal> pMaterial3 = std::make_shared<Metal>(Vec3 { 0.7f, 0.6f, 0.5f }, 0.f);
    world.Add(std::make_unique<Sphere>(Vec3 { 4.f, 1.f, 0.f }, 1.f, pMaterial3));

    Camera camera = { kWidth, kHeight };
    camera.SetSample(500);
    camera.SetMaxDepth(50);
    camera.SetFOV(ToRad(20.f));
    camera.SetCenter({ 13.f, 2.f, -3.f });
    camera.SetLookAt({ 0.f, 0.f, 0.f });
    camera.SetCameraUp(Vec3::kUp);
    camera.SetDefocusRad(ToRad(0.6f));
    camera.SetFocusDistance(10.f);
    camera.Render(world);
    SaveImageBuffer(camera.GetImageBuffer());
    return 0;
}