#include "PAZ_IO"
#include <fstream>

// Need to add support for comments and binary (`P4`) PBMs.
paz::Image<std::uint8_t, 1> paz::load_pbm(const std::string& path)
{
    std::ifstream in(path);
    if(!in)
    {
        throw std::runtime_error("Failed to open input file \"" + path + "\".");
    }

    // Check format.
    char c0, c1;
    in >> c0;
    in >> c1;
    if(c0 != 'P' || c1 != '1')
    {
        throw std::runtime_error("This is not a plain PBM file.");
    }

    // Get dimensions.
    unsigned int width, height;
    in >> width;
    in >> height;

    // Get data.
    std::vector<std::uint8_t> pixels(width*height, 0);
    char c;
    unsigned int n = 0;
    while(in >> c && n < width*height)
    {
        if(c == '0')
        {
            unsigned int x = n%width;
            unsigned int y = n/width;
            pixels[width*(height - 1 - y) + x] = 255;
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

    return paz::Image<std::uint8_t, 1>(pixels.data(), width, height);
}
