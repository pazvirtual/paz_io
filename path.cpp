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
        const std::filesystem::path p(path);
        const auto f = p.filename();
        std::string ext = f.extension().string();
        // Remove period from extension.
        if(!ext.empty())
        {
            ext.erase(0, 1);
        }
        return {p.parent_path().string(), f.stem().string(), ext};
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("Failed to split path: " + std::string(e.
            what()));
    }
}

#endif
