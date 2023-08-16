#include "PAZ_IO"
#include <array>
#include <fstream>
#include <cmath>

paz::Image<std::uint8_t, 4> paz::parse_bmp(const Bytes& content)
{
    if(content[0] != 'B' || content[1] != 'M')
    {
        throw std::runtime_error("Not a BMP.");
    }
    throw std::logic_error("NOT IMPLEMENTED");
}

paz::Bytes paz::to_bmp(const Image<std::uint8_t, 4>& img)
{
    unsigned int extraBytes = 4 - ((3*img.width())%4);
    if(extraBytes == 4)
    {
        extraBytes = 0;
    }

    const unsigned int paddedSize = (3*img.width() + extraBytes)*img.height();
    const std::array<unsigned int, 13> headers = {paddedSize + 54u, 0, 54, 40,
        static_cast<unsigned int>(img.width()), static_cast<unsigned int>(img.
        height()), 0, 0, paddedSize, 0, 0, 0, 0};

    Bytes res("BM");

    for(int i = 0; i < 6; ++i)
    {
        res.push_back(headers[i]&0x000000ffu);
        res.push_back((headers[i]&0x0000ff00u) >> 8);
        res.push_back((headers[i]&0x00ff0000u) >> 16);
        res.push_back((headers[i]&0xff000000u) >> 24);
    }

    res.push_back(1);
    res.push_back(0);
    res.push_back(24);
    res.push_back(0);

    for(int i = 7; i < 13; ++i)
    {
        res.push_back(headers[i]&0x000000ffu);
        res.push_back((headers[i]&0x0000ff00u) >> 8);
        res.push_back((headers[i]&0x00ff0000u) >> 16);
        res.push_back((headers[i]&0xff000000u) >> 24);
    }

    for(int y = 0; y < img.height(); ++y)
    {
        for(int x = 0; x < img.width(); ++x)
        {
            for(int i = 2; i >= 0; --i)
            {
                res.push_back(img[4*(y*img.width() + x) + i]);
            }
        }
        for(unsigned int i = 0; i < extraBytes; ++i)
        {
            res.push_back(0);
        }
    }

    return res;
}
