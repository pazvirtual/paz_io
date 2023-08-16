#include "PAZ_IO"
#include <cstring>

paz::Bytes::Bytes(const char* str) : Bytes(str, str + std::strlen(str)) {}

paz::Bytes::Bytes(const std::string& str) : Bytes(str.begin(), str.end()) {}

std::string paz::Bytes::str() const
{
    return std::string(begin(), end());
}
