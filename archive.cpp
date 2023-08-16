#include "PAZ_IO"
#include "miniz.h"
#include <fstream>
#include <sstream>
#include <map>

#ifdef compress
#undef compress
#endif
#ifdef uncompress
#undef uncompress
#endif

#define CASE_STRING(x) case x: return #x;

// This upper-bound-calculating function taken from Miniz.
static std::size_t compress_bound(const std::size_t srcLen)
{
    return std::max(128 + (srcLen*110)/100, 128 + srcLen + ((srcLen/(31*1024)) +
        1)*5);
}

std::string mz_status(int status) noexcept
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
        default: return "Status code " + std::to_string(status) + " not recogni"
            "zed";
    }
}

paz::Bytes paz::compress(const Bytes& src)
{
    // Check if this is too much data for Miniz to handle in one shot.
    const std::size_t cmpLenTemp = compress_bound(src.size());
    if(cmpLenTemp > std::numeric_limits<unsigned long>::max())
    {
        throw std::invalid_argument("Buffer exceeds maximum length for compress"
            "ion (" + std::to_string(cmpLenTemp) + "/" + std::to_string(std::
            numeric_limits<unsigned long>::max()) + ")");
    }

    // Compress output. (First four bytes are used to store uncompressed size).
    const unsigned long srcLen = src.size();
    unsigned long cmpLen = cmpLenTemp;

    Bytes buf(cmpLen + 4);

    for(unsigned long i = 0; i < 4; ++i)
    {
        buf[i] = static_cast<unsigned char>(srcLen >> 8*i);
    }

    const int status = mz_compress2(buf.data() + 4, &cmpLen, src.data(), srcLen,
        MZ_BEST_COMPRESSION);
    if(status != MZ_OK)
    {
        throw std::runtime_error("Compression failed: " + mz_status(status) +
            ".");
    }

    buf.resize(cmpLen + 4);

    return buf;
}

paz::Bytes paz::uncompress(const Bytes& src)
{
    // Extract size of uncompressed data.
    const unsigned long srcLen = src.size();
    unsigned long destLen = 0;
    for(unsigned long i = 0; i < 4; ++i)
    {
        destLen |= static_cast<unsigned long>(src[i]) << 8*i;
    }

    if(!destLen)
    {
        return "";
    }

    // Decompress input.
    Bytes buf(destLen);
    const int status = mz_uncompress(buf.data(), &destLen, src.data() + 4,
        srcLen - 4);
    if(status != MZ_OK)
    {
        throw std::runtime_error("Decompression failed: " + mz_status(status) +
            ".");
    }

    return buf;
}

paz::Archive::Archive(const std::string& path) : Archive(load_file(path)) {}

paz::Archive::Archive(const Bytes& src)
{
    if(src.empty())
    {
        throw std::runtime_error("Archive is empty.");
    }

    // Get contents list.
    unsigned long dataStart = 0;
    for(unsigned long i = 0; i < 4; ++i)
    {
        dataStart |= static_cast<unsigned long>(src[i]) << 8*i;
    }
    std::stringstream ss(uncompress(Bytes(src.begin() + 4, src.begin() + 4 +
        dataStart)).str());
    std::map<std::size_t, std::string> contents;
    std::string line;
    while(std::getline(ss, line))
    {
        std::stringstream lineSs(line);
        std::string name;
        std::getline(lineSs, name, ' ');
        std::string str;
        std::getline(lineSs, str);
        contents[4 + dataStart + std::stoull(str)] = name;
    }

    // Get compressed blocks.
    for(auto it = contents.begin(); it != contents.end(); ++it)
    {
        if(_blocks.count(it->second))
        {
            throw std::runtime_error("Archive already contains block \"" + it->
                second + "\".");
        }
        auto next = it;
        ++next;
        _blocks.try_emplace(it->second, src.begin() + it->first, next ==
            contents.end() ? src.end() : src.begin() + next->first);
    }
}

void paz::Archive::add(const std::string& name, const Bytes& data)
{
    if(_blocks.count(name))
    {
        throw std::runtime_error("Archive already contains block \"" + name +
            "\".");
    }

    _blocks[name] = compress(data);
}

paz::Bytes paz::Archive::get(const std::string& name) const
{
    if(!_blocks.count(name))
    {
        throw std::runtime_error("Archive does not contain block \"" + name +
            "\".");
    }

    return uncompress(_blocks.at(name));
}

void paz::Archive::write(const std::string& path) const
{
    // Construct contents list.
    std::ostringstream contents;
    std::size_t curOutByte = 0;
    for(const auto& n : _blocks)
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
    const Bytes buf = compress(contents.str());
    const unsigned long dataStart = buf.size();
    for(unsigned long i = 0; i < 4; ++i)
    {
        out << static_cast<unsigned char>(dataStart >> 8*i);
    }
    out.write(reinterpret_cast<char*>(const_cast<unsigned char*>(buf.data())),
        buf.size());

    // Write blocks to file.
    for(const auto& n : _blocks)
    {
        out.write(reinterpret_cast<char*>(const_cast<unsigned char*>(n.second.
            data())), n.second.size());
    }
}

void paz::Archive::clear()
{
    _blocks.clear();
}

std::unordered_set<std::string> paz::Archive::contents() const
{
    std::unordered_set<std::string> contentsList;
    contentsList.reserve(_blocks.size());
    for(auto& n : _blocks)
    {
        contentsList.insert(n.first);
    }
    return contentsList;
}
