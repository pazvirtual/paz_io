#include "PAZ_IO"
#include <sstream>

// Need to add support for comments and binary (`P4`) PBMs.
paz::Image<std::uint8_t, 1> paz::parse_pbm(const Bytes& content)
{
    // Check format.
    if(content[0] != 'P' || content[1] != '1')
    {
        throw std::runtime_error("This is not a plain PBM.");
    }

    // Create input stream.
    std::istringstream iss(content.str().substr(2));

    // Get dimensions.
    unsigned int width, height;
    iss >> width;
    iss >> height;

    // Get data.
    Image<std::uint8_t, 1> image(width, height);
    std::fill(image.begin(), image.end(), 0);
    char c;
    unsigned int n = 0;
    while(iss >> c && n < width*height)
    {
        if(c == '0')
        {
            unsigned int x = n%width;
            unsigned int y = n/width;
            image[width*(height - 1 - y) + x] = 255;
        }
        if(c == '0' || c == '1')
        {
            ++n;
        }
    }

    // Check that we got all the pixels.
    if(n != width*height)
    {
        throw std::runtime_error("Number of pixels does not match dimensions.");
    }

    return image;
}
