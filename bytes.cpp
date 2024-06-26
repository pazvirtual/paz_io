#include "PAZ_IO"
#include <cstring>
#include <fstream>
#include <algorithm>

paz::Bytes::Bytes(const char* str) : Bytes(str, str + std::strlen(str)) {}

paz::Bytes::Bytes(const std::string& str) : Bytes(str.begin(), str.end()) {}

std::string paz::Bytes::str() const
{
    std::string s(begin(), end());

    // Replace all `\r` not followed by `\n` with `\n`.
    for(std::size_t i = 0; i < s.size(); ++i)
    {
        if(s[i] == '\r' && (i + 1 == s.size() || s[i + 1] != '\n'))
        {
            s[i] = '\n';
        }
    }

    // Remove all remaining `\r`.
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
    s.shrink_to_fit();

    return s;
}

paz::Bytes paz::read_bytes(const std::string& path)
{
    std::ifstream in(path, std::ios::binary);
    if(!in)
    {
        throw std::runtime_error("Failed to open input file \"" + path + "\".");
    }
    return Bytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<
        char>());
}

void paz::write_bytes(const std::string& path, const Bytes& data)
{
    std::ofstream out(path, std::ios::binary);
    if(!out)
    {
        throw std::runtime_error("Failed to open output file \"" + path + "\"."
            );
    }
    std::copy(data.begin(), data.end(), std::ostreambuf_iterator<char>(out));
}
