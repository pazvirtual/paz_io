#include "PAZ_IO"
#include <sstream>
#include <fstream>

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

void paz::write_pbm(const std::string& path, const Image<std::uint8_t, 1>& img)
{
    std::ofstream out(path, std::ios::binary);
    if(!out)
    {
        throw std::runtime_error("Unable to open \"" + path + "\".");
    }

    out << "P4\n" << img.width() << " " << img.height() << "\n";
    for(int i = 0; i < img.height(); ++i)
    {
        unsigned int rowStart = (img.height() - i - 1)*img.width();
        int x = 0;
        unsigned char c = 0;
        while(true)
        {
            if(img[rowStart + x] < 128)
            {
                c |= 1 << (7 - x%8);
            }
            ++x;
            if(x == img.width())
            {
                out.put(c);
                break;
            }
            if(!(x%8))
            {
                out.put(c);
                c = 0;
            }
        }
    }
}

// Need to add support for comments.
paz::Image<std::uint8_t, 1> paz::parse_pbm(const Bytes& content)
{
    // Check format.
    if(content[0] != 'P' || !(content[1] == '1' || content[1] == '4'))
    {
        throw std::runtime_error("Not a PBM.");
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
