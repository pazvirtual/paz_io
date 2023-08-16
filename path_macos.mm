#include "detect_os.hpp"

#ifdef PAZ_MACOS

#include "PAZ_IO"
#import <Foundation/Foundation.h>

std::array<std::string, 3> paz::split_path(const std::string& path)
{
    if(path.empty())
    {
        return {};
    }
    try
    {
        @try
        {
            NSURL* p = [NSURL URLWithString:[NSString stringWithUTF8String:path.
                c_str()]];
            NSString* f = [p lastPathComponent];
            const std::string dir = [[[p URLByDeletingLastPathComponent] path]
                UTF8String];
            const std::string name = [[f stringByDeletingPathExtension]
                UTF8String];
            const std::string ext = [[f pathExtension] UTF8String];
            [p release];
            [f release];
            return {dir, name, ext};
        }
        @catch(NSException* e)
        {
            throw std::runtime_error([[NSString stringWithFormat:@"%@", e]
                UTF8String]);
        }
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("Failed to split path: " + std::string(e.
            what()));
    }
}

#endif
