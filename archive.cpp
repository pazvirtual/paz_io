#include "PAZ_IO"
#include "miniz.h"
#include <fstream>
#include <sstream>

#ifdef compress
#undef compress
#endif
#ifdef uncompress
#undef uncompress
#endif

#define CASE_STRING(x) case x: return #x;

// This upper-bound-calculating function taken from Miniz.
std::size_t compress_bound(const std::size_t srcLen)
{
    return std::max(128 + (srcLen*110)/100, 128 + srcLen + ((srcLen/(31*1024)) +
        1)*5);
}

std::string mz_status(int status)
{
    switch(status)
    {
        CASE_STRING(Z_OK)
        CASE_STRING(Z_STREAM_END)
        CASE_STRING(Z_NEED_DICT)
        CASE_STRING(Z_ERRNO)
        CASE_STRING(Z_STREAM_ERROR)
        CASE_STRING(Z_DATA_ERROR)
        CASE_STRING(Z_MEM_ERROR)
        CASE_STRING(Z_BUF_ERROR)
        CASE_STRING(Z_VERSION_ERROR)
        CASE_STRING(Z_PARAM_ERROR)
    }
    return "Status code " + std::to_string(status) + " not recognized";
}

std::vector<unsigned char> paz::compress(const std::string& str)
{
    // Check if this is too much data for Miniz to handle in one shot.
    const std::size_t cmpLenTemp = compress_bound(str.length());
    if(cmpLenTemp > std::numeric_limits<unsigned long>::max())
    {
        throw std::invalid_argument("String exceeds maximum length for compress"
            "ion (" + std::to_string(cmpLenTemp) + "/" + std::to_string(std::
            numeric_limits<unsigned long>::max()) + ")");
    }

    // Compress output. (First four bytes are used to store uncompressed size).
    const char* outStr = str.c_str();
    const unsigned long srcLen = str.length();
    unsigned long cmpLen = cmpLenTemp;

    std::vector<unsigned char> buf(cmpLen + 4);

    for(unsigned long i = 0; i < 4; ++i)
    {
        buf[i] = (unsigned char)(srcLen >> 8*i);
    }

    const int status = mz_compress2(buf.data() + 4, &cmpLen, (const unsigned
        char*)outStr, srcLen, MZ_BEST_COMPRESSION);
    if(status != MZ_OK)
    {
        throw std::runtime_error("Compression failed: " + mz_status(status) +
            ".");
    }

    buf.resize(cmpLen + 4);

    return buf;
}

std::string paz::uncompress(const std::vector<unsigned char>& src)
{
    // Extract size of uncompressed data.
    const unsigned long srcLen = src.size();
    unsigned long destLen = 0;
    for(unsigned long i = 0; i < 4; ++i)
    {
        destLen |= (unsigned long)src[i] << 8*i;
    }

    if(!destLen)
    {
        return "";
    }

    // Decompress input.
    std::vector<unsigned char> buf(destLen);
    const int status = mz_uncompress(buf.data(), &destLen, src.data() + 4,
        srcLen - 4);
    if(status != MZ_OK)
    {
        throw std::runtime_error("Decompression failed: " + mz_status(status) +
            ".");
    }

    return std::string(buf.begin(), buf.end());
}

void paz::write_archive(const std::string& path, const std::unordered_map<std::
    string, std::vector<unsigned char>>& blocks)
{
    // Construct contents list.
    std::ostringstream contents;
    std::size_t curOutByte = 0;
    for(const auto& n : blocks)
    {
        if(n.first.empty())
        {
            throw std::runtime_error("Output block has no name.");
        }
        contents << n.first << " " << curOutByte << std::endl;
        curOutByte += n.second.size();
    }

    // Open output file.
    std::ofstream out(path, std::ios::binary);
    if(!out)
    {
        throw std::runtime_error("Failed to open output file \"" + path +
            "\".");
    }

    // Compress and write contents list to file.
    const std::vector<unsigned char> buf = paz::compress(contents.str());
    const unsigned long dataStart = buf.size();
    for(unsigned long i = 0; i < 4; ++i)
    {
        out << (unsigned char)(dataStart >> 8*i);
    }
    out.write((char*)buf.data(), buf.size());

    // Write blocks to file.
    for(const auto& n : blocks)
    {
        out.write((char*)n.second.data(), n.second.size());
    }
}

std::unordered_map<std::string, std::size_t> paz::load_contents_list(const std::
    string& path)
{
    // Open input file.
    std::ifstream file(path, std::ios::binary);
    if(!file)
    {
        throw std::runtime_error("Failed to open input file \"" + std::string(
            path) + "\".");
    }

    // Extract input data and close file.
    std::vector<unsigned char> src((std::istreambuf_iterator<char>(file)), std::
        istreambuf_iterator<char>());
    file.close();
    if(!src.size())
    {
        throw std::runtime_error("Archive \"" + std::string(path) + "\" is empt"
            "y.");
    }

    // Get contents list.
    unsigned long dataStart = 0;
    for(unsigned long i = 0; i < 4; ++i)
    {
        dataStart |= (unsigned long)src[i] << 8*i;
    }
    std::stringstream ss(paz::uncompress(std::vector<unsigned char>(src.begin()
        + 4, src.begin() + 4 + dataStart)));
    std::unordered_map<std::string, std::size_t> contents;
    std::string line;
    while(std::getline(ss, line))
    {
        std::stringstream lineSs(line);
        std::string name;
        std::getline(lineSs, name, ' ');
        std::string str;
        std::getline(lineSs, str);
        contents[name] = 4 + dataStart + std::stoull(str);
    }

    return contents;
}

std::string paz::load_block(const std::string& path, const std::string& name)
{
    // Open input file.
    std::ifstream file(path, std::ios::binary);
    if(!file)
    {
        throw std::runtime_error("Unable to open input file \"" + std::string(
            path) + "\".");
    }

    // Extract input data and close file.
    std::vector<unsigned char> src((std::istreambuf_iterator<char>(file)), std::
        istreambuf_iterator<char>());
    file.close();
    if(!src.size())
    {
        throw std::runtime_error("Archive \"" + path + "\" is empty.");
    }

    // Find the desired block.
    unsigned long dataStart = 0;
    for(unsigned long i = 0; i < 4; ++i)
    {
        dataStart |= (unsigned long)src[i] << 8*i;
    }
    std::stringstream ss(paz::uncompress(std::vector<unsigned char>(src.begin()
        + 4, src.begin() + 4 + dataStart)));
    std::string line;
    std::size_t start = 0;
    std::size_t end = src.size();
    while(std::getline(ss, line))
    {
        std::stringstream lineSs(line);
        std::string curName;
        std::getline(lineSs, curName, ' ');
        std::string str;
        std::getline(lineSs, str);
        const std::size_t s = std::stoull(str);
        if(start)
        {
            end = 4 + dataStart + s;
            break;
        }
        else if(curName == name)
        {
            start = 4 + dataStart + s;
        }
    }
    if(!start)
    {
        throw std::runtime_error("Archive \"" + path + "\" does not contain blo"
            "ck \"" + name + "\".");
    }

    return uncompress(std::vector<unsigned char>(src.begin() + start, src.
        begin() + end));
}
