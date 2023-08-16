#include "PAZ_IO"
#include <sstream>

// Need to add support for comments.
paz::Image<std::uint8_t, 1> paz::parse_pbm(const Bytes& content)
{
    // Check format.
    if(content[0] != 'P' || !(content[1] == '1' || content[1] == '4'))
    {
        throw std::runtime_error("This is not a plain PBM.");
    }
    const bool isBinary = content[1] == '4';

    // Create input stream.
    std::istringstream iss(content.str().substr(2));

    // Get dimensions.
    unsigned int width, height;
    iss >> width;
    iss >> height;

    // Get data.
    Image<std::uint8_t, 1> image(width, height);
    std::fill(image.begin(), image.end(), 0);
    unsigned int n = 0;
    if(isBinary)
    {
        std::uint8_t b;
        while(iss >> b && n < width*height)
        {
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
        char c;
        while(iss >> c && n < width*height)
        {
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
        throw std::runtime_error("Number of pixels does not match dimensions.");
    }

    return image;
}
