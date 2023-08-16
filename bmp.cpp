#include "PAZ_IO"
#include <array>
#include <fstream>
#include <cmath>

void paz::write_bmp(const std::string& path, const Image<float, 3>& image)
{
    unsigned int extraBytes = 4 - ((3*image.width())%4);
    if(extraBytes == 4)
    {
        extraBytes = 0;
    }

    const unsigned int paddedSize = (3*image.width() + extraBytes)*image.
        height();
    const std::array<unsigned int, 13> headers = {paddedSize + 54u, 0, 54, 40,
        static_cast<unsigned int>(image.width()), static_cast<unsigned int>(
        image.height()), 0, 0, paddedSize, 0, 0, 0, 0};

    std::ofstream out(path, std::ios::binary);
    if(!out)
    {
        throw std::runtime_error("Unable to open \"" + path + "\".");
    }

    out << "BM";

    for(int i = 0; i < 6; ++i)
    {
        out.put(headers[i]&0x000000ffu);
        out.put((headers[i]&0x0000ff00u) >> 8);
        out.put((headers[i]&0x00ff0000u) >> 16);
        out.put((headers[i]&0xff000000u) >> 24);
    }

    out.put(1);
    out.put(0);
    out.put(24);
    out.put(0);

    for(int i = 7; i < 13; ++i)
    {
        out.put(headers[i]&0x000000ffu);
        out.put((headers[i]&0x0000ff00u) >> 8);
        out.put((headers[i]&0x00ff0000u) >> 16);
        out.put((headers[i]&0xff000000u) >> 24);
    }

    for(int y = 0; y < image.height(); ++y)
    {
        for(int x = 0; x < image.width(); ++x)
        {
            for(int i = 2; i >= 0; --i)
            {
                out.put(std::min(static_cast<unsigned int>(std::round(std::max(
                    image[3*(y*image.width() + x) + i], 0.f)*255.f)), 255u));
            }
        }
        for(unsigned int i = 0; i < extraBytes; ++i)
        {
            out.put(0);
        }
    }
}
