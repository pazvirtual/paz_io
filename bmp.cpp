#include "PAZ_IO"
#include <array>
#include <fstream>
#include <cmath>

paz::Image<std::uint8_t, 4> paz::parse_bmp(const Bytes& content)
{
    std::string fmt;
    if(content.size() >= 15)
    {
        fmt = std::string(content.begin(), content.begin() + 2);
    }
    if(fmt != "BM" && fmt != "BA" && fmt != "CI" && fmt != "CP" && fmt != "IC"
        && fmt != "PT")
    {
        throw std::runtime_error("Not a bitmap image.");
    }
    if(fmt != "BM")
    {
        throw std::runtime_error("Unsupported bitmap format " + fmt + ".");
    }
    std::size_t start = 0;
    for(int i = 0; i < 4; ++i)
    {
        start |= content[10 + i] << 8*i;
    }
    std::size_t headerSize = 0;
    for(int i = 0; i < 4; ++i)
    {
        headerSize |= content[14 + i] << 8*i;
    }
    if(headerSize != 40)
    {
        throw std::runtime_error("Unsupported DIB header format (must be BITMAP"
            "INFOHEADER).");
    }
    long width = 0;
    for(int i = 0; i < 4; ++i)
    {
        width |= content[18 + i] << 8*i;
    }
    long height = 0;
    for(int i = 0; i < 4; ++i)
    {
        height |= content[22 + i] << 8*i;
    }
    //TEMP - need to check compression method, color depth, and palette
    Image<std::uint8_t, 4> res(width, height);
    unsigned int extraBytes = 4 - ((3*width)%4);
    if(extraBytes == 4)
    {
        extraBytes = 0;
    }
    for(int i = 0; i < height; ++i)
    {
        for(int j = 0; j < width; ++j)
        {
            for(int k = 0; k < 3; ++k)
            {
                res[4*(width*i + j) + k] = content[start + (3*width +
                    extraBytes)*i + 3*j + 2 - k];
            }
            res[4*(width*i + j) + 3] = 255;
        }
    }
    return res;
}

paz::Bytes paz::to_bmp(const Image<std::uint8_t, 4>& img)
{
    unsigned int extraBytes = 4 - ((3*img.width())%4);
    if(extraBytes == 4)
    {
        extraBytes = 0;
    }

    const unsigned long paddedSize = (3*img.width() + extraBytes)*img.height();
    const std::array<unsigned long, 13> headers = {paddedSize + 54u, 0, 54, 40,
        static_cast<unsigned long>(img.width()), static_cast<unsigned long>(img.
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
