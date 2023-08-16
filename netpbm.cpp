#include "PAZ_IO"
#include <sstream>
#include <fstream>

static void skip(const paz::Bytes& content, std::size_t& idx)
{
    bool inComment = false;
    while(idx < content.size())
    {
        if(inComment)
        {
            if(content[idx] == '\n')
            {
                inComment = false;
            }
            ++idx;
        }
        else
        {
            if(std::isspace(content[idx]))
            {
                ++idx;
            }
            else if(content[idx] == '#')
            {
                inComment = true;
                ++idx;
            }
            else
            {
                return;
            }
        }
    }
}

static unsigned int get_dim(const paz::Bytes& content, std::size_t& idx)
{
    std::stringstream ss;
    while(idx < content.size() && !std::isspace(content[idx]))
    {
        ss << content[idx];
        ++idx;
    }
    unsigned int dim;
    ss >> dim;
    return dim;
}

paz::Bytes paz::to_pbm(const Image& img)
{
    if(img.format() != ImageFormat::R8UNorm)
    {
        throw std::runtime_error("Image format must be R8UNorm.");
    }

    Bytes res("P4\n" + std::to_string(img.width()) + " " + std::to_string(img.
        height()) + "\n");
    for(int i = 0; i < img.height(); ++i)
    {
        unsigned int rowStart = (img.height() - i - 1)*img.width();
        int x = 0;
        unsigned char c = 0;
        while(true)
        {
            if(img.bytes()[rowStart + x] < 128)
            {
                c |= 1 << (7 - x%8);
            }
            ++x;
            if(x == img.width())
            {
                res.push_back(c);
                break;
            }
            if(!(x%8))
            {
                res.push_back(c);
                c = 0;
            }
        }
    }
    return res;
}

// Need to add support for comments.
paz::Image paz::parse_pbm(const Bytes& content)
{
    // Check format.
    if(content[0] != 'P' || !(content[1] == '1' || content[1] == '4'))
    {
        throw std::runtime_error("Not a PBM.");
    }
    const bool isBinary = content[1] == '4';

    // Get dimensions.
    std::size_t idx = 2;
    skip(content, idx);
    const unsigned int width = get_dim(content, idx);
    skip(content, idx);
    const unsigned int height = get_dim(content, idx);
    skip(content, idx);

    // Get data.
    Image res(ImageFormat::R8UNorm, width, height);
    std::fill(res.bytes().begin(), res.bytes().end(), 0);
    unsigned int n = 0;
    if(isBinary)
    {
        while(idx < content.size() && n < width*height)
        {
            const std::uint8_t b = content[idx++];
            for(int i = 0; i < 8; ++i)
            {
                const unsigned int x = n%width;
                const unsigned int y = n/width;
                res.bytes()[width*(height - 1 - y) + x] = !((b >> (7 - i))&1)*
                    255;
                ++n;
                if(n == width*(y + 1))
                {
                    break;
                }
            }
        }
    }
    else
    {
        while(idx < content.size() && n < width*height)
        {
            const char c = content[idx++];
            if(c == '0')
            {
                const unsigned int x = n%width;
                const unsigned int y = n/width;
                res.bytes()[width*(height - 1 - y) + x] = 255;
            }
            if(c == '0' || c == '1')
            {
                ++n;
            }
        }
    }

    // Check that we got all the pixels.
    if(n != width*height)
    {
        throw std::runtime_error("Got " + std::to_string(n) + " pixels, expecte"
            "d at least " + std::to_string(width*height) + ".");
    }

    return res;
}
