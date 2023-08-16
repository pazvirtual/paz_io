#include "PAZ_IO"
#include <array>
#include <fstream>
#include <cmath>

paz::Image paz::parse_bmp(const Bytes& content)
{
    std::string fmt;
    if(content.size() > 1)
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
        throw std::runtime_error("Unsupported DIB header format " + std::
            to_string(headerSize) + " (must be 40/BITMAPINFOHEADER).");
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
    Image res(ImageFormat::RGBA8UNorm_sRGB, width, height);
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
                res.bytes()[4*(width*i + j) + k] = content[start + (3*width +
                    extraBytes)*i + 3*j + 2 - k];
            }
            res.bytes()[4*(width*i + j) + 3] = 255;
        }
    }
    return res;
}

paz::Bytes paz::to_bmp(const Image& img)
{
    if(img.format() != ImageFormat::RGBA8UNorm_sRGB)
    {
        throw std::runtime_error("Image format must be RGBA8UNorm_sRGB.");
    }

    unsigned int extraBytes = 4 - ((3*img.width())%4);
    if(extraBytes == 4)
    {
        extraBytes = 0;
    }

    const unsigned long paddedSize = (3*img.width() + extraBytes)*img.height();
    const std::array<unsigned long, 13> headers = {paddedSize + 54u, 0, 54, 40,
        static_cast<unsigned long>(img.width()), static_cast<unsigned long>(img.
        height()), 0, 0, paddedSize, 0, 0, 0, 0};

    Bytes res(54 + paddedSize);
    res[0] = 'B';
    res[1] = 'M';

    for(int i = 0; i < 6; ++i)
    {
        res[2 + 4*i + 0] = headers[i]&0x000000ffu;
        res[2 + 4*i + 1] = (headers[i]&0x0000ff00u) >> 8;
        res[2 + 4*i + 2] = (headers[i]&0x00ff0000u) >> 16;
        res[2 + 4*i + 3] = (headers[i]&0xff000000u) >> 24;
    }

    res[26] = 1;
    res[27] = 0;
    res[28] = 24;
    res[29] = 0;

    for(int i = 7; i < 13; ++i)
    {
        res[2 + 4*i + 0] = headers[i]&0x000000ffu;
        res[2 + 4*i + 1] = (headers[i]&0x0000ff00u) >> 8;
        res[2 + 4*i + 2] = (headers[i]&0x00ff0000u) >> 16;
        res[2 + 4*i + 3] = (headers[i]&0xff000000u) >> 24;
    }

    for(int y = 0; y < img.height(); ++y)
    {
        for(int x = 0; x < img.width(); ++x)
        {
            for(int i = 0; i < 3; ++i)
            {
                res[54 + (3*img.width() + extraBytes)*y + 3*x + i] = img.
                    bytes()[4*(y*img.width() + x) + 2 - i];
            }
        }
        for(unsigned int i = 0; i < extraBytes; ++i)
        {
            res[54 + (3*img.width() + extraBytes)*y + 3*img.width() + i] = 0;
        }
    }

    return res;
}
