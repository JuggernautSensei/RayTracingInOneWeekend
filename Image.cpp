#include "pch.h"
#include "Image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

RGBImageBuffer::RGBImageBuffer(const Int32 _width, const Int32 _height)
    : m_width(_width)
    , m_height(_height)
{
    m_pixels = std::make_unique<UInt8[]>(m_width * m_height * 3);
}

bool RGBImageBuffer::SaveToFile(
    const fs::path& _path) const
{
    fs::create_directories(_path.parent_path());

    const std::string path = _path.string();
    if (stbi_write_png(path.c_str(), m_width, m_height, 3, m_pixels.get(), m_width * 3) == NULL)
    {
        std::cerr << std::format(R"([FAILD] Failed to save image file: "{}")", path) << "\n";
        return false;
    }

    std::cout << std::format(R"([SUCCESS] Image file saved: "{}")", path) << "\n";
    return true;
}

void RGBImageBuffer::WriteLinear(const Int32 _x, const Int32 _y, const Vec3 _color)
{
    const Vec3  rgb      = _color.Clamp(Vec3::kZero, Vec3::kOne);
    const Int32 offset   = 3 * (m_width * _y + _x);
    m_pixels[offset + 0] = static_cast<UInt8>(255.f * rgb.r);
    m_pixels[offset + 1] = static_cast<UInt8>(255.f * rgb.g);
    m_pixels[offset + 2] = static_cast<UInt8>(255.f * rgb.b);
}

void RGBImageBuffer::WriteRGB(const Int32 _x, const Int32 _y, const UInt8 _r, const UInt8 _g, const UInt8 _b)
{
    const Int32 offset   = 3 * (m_width * _y + _x);
    m_pixels[offset + 0] = _r;
    m_pixels[offset + 1] = _g;
    m_pixels[offset + 2] = _b;
}

UInt8* RGBImageBuffer::GetPixels()
{
    return m_pixels.get();
}

const UInt8* RGBImageBuffer::GetPixels() const
{
    return m_pixels.get();
}

Int32 RGBImageBuffer::GetWidth() const
{
    return m_width;
}

Int32 RGBImageBuffer::GetHeight() const
{
    return m_height;
}