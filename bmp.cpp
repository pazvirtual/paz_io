#include "PAZ_IO"
#include <array>
#include <fstream>
#include <cmath>

void paz::write_bmp(const std::string& path, unsigned int width, const std::
    vector<float>& rgb)
{
    unsigned int extraBytes = 4 - ((3*width)%4);
    if(extraBytes == 4)
    {
        extraBytes = 0;
    }

    const unsigned int height = rgb.size()/(3*width);
    const unsigned int paddedSize = (3*width + extraBytes)*height;
    const std::array<unsigned int, 13> headers =
    {
        paddedSize + 54, 0, 54, 40, width, height, 0, 0, paddedSize, 0, 0, 0, 0
    };

    std::ofstream out(path, std::ios::binary);
    if(!out)
    {
        throw std::runtime_error("Unable to open \"" + path + "\".");
    }

    out << "BM";

    for(int i = 0; i < 6; ++i)
    {
        out.put(headers[i]&0x000000ff);
        out.put((headers[i]&0x0000ff00) >> 8);
        out.put((headers[i]&0x00ff0000) >> 16);
        out.put((headers[i]&(unsigned int)0xff000000) >> 24);
    }

    out.put(1);
    out.put(0);
    out.put(24);
    out.put(0);

    for(int i = 7; i < 13; ++i)
    {
        out.put(headers[i]&0x000000ff);
        out.put((headers[i]&0x0000ff00) >> 8);
        out.put((headers[i]&0x00ff0000) >> 16);
        out.put((headers[i]&(unsigned int)0xff000000) >> 24);
    }

    for(unsigned int y = 0; y < height; ++y)
    {
        for(unsigned int x = 0; x < width; ++x)
        {
            for(int i = 2; i >= 0; --i)
            {
                out.put(std::min((unsigned int)std::round(std::max(rgb[3*(y*
                    width + x) + i], 0.f)*255.f), 255u));
            }
        }
        for(unsigned int i = 0; i < extraBytes; ++i)
        {
            out.put(0);
        }
    }
}
