#include "PAZ_IO"
#include <cstring>
#include <fstream>

paz::Bytes::Bytes(const char* str) : Bytes(str, str + std::strlen(str)) {}

paz::Bytes::Bytes(const std::string& str) : Bytes(str.begin(), str.end()) {}

std::string paz::Bytes::str() const
{
    return std::string(begin(), end());
}

paz::Bytes paz::load_file(const std::string& path)
{
    std::ifstream in(path, std::ios::binary);
    if(!in)
    {
        throw std::runtime_error("Failed to open input file \"" + path + "\".");
    }
    return Bytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<
        char>());
}
