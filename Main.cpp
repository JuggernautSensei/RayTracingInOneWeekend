#include <iostream>
#include <vector>
#include <sstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main()
{
    constexpr int kWidth  = 256;
    constexpr int kHeight = 256;

    std::vector<uint8_t> buf(3 * kWidth * kHeight);
    for (int i = 0; i < kHeight; ++i)
    {
        const float progress = static_cast<float>(i) / static_cast<float>(kHeight - 1);
        std::cout << "image creating " << progress * 100 << "%...\n";

        for (int j = 0; j < kWidth; ++j)
        {
            const float   u = static_cast<float>(j) / static_cast<float>(kWidth - 1);
            const float   v = static_cast<float>(i) / static_cast<float>(kHeight - 1);
            const uint8_t r = static_cast<uint8_t>(255.f * u);
            const uint8_t g = static_cast<uint8_t>(255.f * v);
            const uint8_t b = 0;

            const int offset = 3 * (kWidth * i + j);
            buf[offset + 0]  = r;
            buf[offset + 1]  = g;
            buf[offset + 2]  = b;
        }
    }

    if (stbi_write_png("image.png", kWidth, kHeight, 3, buf.data(), kWidth * 3) == NULL)
    {
        std::cout << "이미지 파일 생성 실패\n";
    }
    else
    {
        std::cout << "이미지 파일 생성 성공\n";
    }

    return 0;
}