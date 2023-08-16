#include "PAZ_IO"
#include <sstream>

static void skip_space(const paz::Bytes& content, std::size_t& idx)
{
    while(idx < content.size() && std::isspace(content[idx]))
    {
        ++idx;
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

// Need to add support for comments.
paz::Image<std::uint8_t, 1> paz::parse_pbm(const Bytes& content)
{
    // Check format.
    if(content[0] != 'P' || !(content[1] == '1' || content[1] == '4'))
    {
        throw std::runtime_error("This is not a PBM.");
    }
    const bool isBinary = content[1] == '4';

    // Get dimensions.
    std::size_t idx = 2;
    skip_space(content, idx);
    const unsigned int width = get_dim(content, idx);
    skip_space(content, idx);
    const unsigned int height = get_dim(content, idx);
    skip_space(content, idx);

    // Get data.
    Image<std::uint8_t, 1> image(width, height);
    std::fill(image.begin(), image.end(), 0);
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
                image[width*(height - 1 - y) + x] = !((b >> (7 - i))&1)*255;
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
                image[width*(height - 1 - y) + x] = 255;
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

    return image;
}
