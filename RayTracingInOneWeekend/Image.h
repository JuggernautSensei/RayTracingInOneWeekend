#pragma once

class RGBImageBuffer
{
public:
    RGBImageBuffer() = default;
    RGBImageBuffer(Int32 _width, Int32 _height);

    bool SaveToPngFile(const fs::path& _path) const;

    void WriteLinear(Int32 _x, Int32 _y, Vec3 _color);
    void WriteRGB(Int32 _x, Int32 _y, UInt8 _r, UInt8 _g, UInt8 _b);

    [[nodiscard]] UInt8*       GetPixels();
    [[nodiscard]] const UInt8* GetPixels() const;
    [[nodiscard]] Int32        GetWidth() const;
    [[nodiscard]] Int32        GetHeight() const;

private:
    std::unique_ptr<UInt8[]> m_pixels = nullptr;
    Int32                    m_width  = 0;
    Int32                    m_height = 0;
};
