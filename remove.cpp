#include "PAZ_IO"

void paz::remove(const std::string& path)
{
    if(std::remove(path.c_str()))
    {
        throw std::runtime_error("Failed to remove \"" + path + "\".");
    }
}
