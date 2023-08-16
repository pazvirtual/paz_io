#include "detect_os.hpp"

#ifndef PAZ_MACOS

#include "PAZ_IO"
#include <filesystem>

std::array<std::string, 3> paz::split_path(const std::string& path)
{
    if(path.empty())
    {
        return {};
    }
    try
    {
        const auto p = std::filesystem::path(path);
        const auto f = p.filename();
        return {p.parent_path().string(), f.stem().string(), f.extension().
            string()};
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("Failed to split path: " + std::string(e.
            what()));
    }
}

#endif
