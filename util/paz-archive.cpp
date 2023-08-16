#include "PAZ_IO"
#include <unordered_set>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

int main(int argc, char** argv)
{
    // Figure out if this is a directory and check for valid arguments.
    std::unordered_set<std::string> files;
    struct stat s;
    bool isDir = (argc == 2 || argc == 3) && !stat(argv[1], &s) && S_ISDIR(s.
        st_mode);
    std::string dirPath;
    if(isDir)
    {
        // Extract bare directory path (no trailing slash) for later.
        dirPath = argv[1];
        while(dirPath.back() == '/')
        {
            dirPath.pop_back();
        }

        // Do not attempt to archive `.` or `..`.
        if(dirPath == "." || dirPath == "..")
        {
            std::cerr << std::endl << "Error: Cannot compress \"" << dirPath <<
                "\"." << std::endl << std::endl;
            return 1;
        }

        DIR* dir;
        struct dirent* ent;
        if((dir = opendir(argv[1])))
        {
            while((ent = readdir(dir)))
            {
                if(ent->d_name[0] != '.')
                {
                    files.insert(ent->d_name);
                }
            }
            closedir(dir);
        }
        else
        {
            std::cerr << std::endl << "Error: Unable to open directory \"" <<
                argv[1] << "\"." << std::endl << std::endl;
            return 1;
        }
    }
    else if(argc < 3)
    {
        std::cout << std::endl << "Usage:" << std::endl << "    paz-archive [fi"
            "le 1] ... [file n] [output path]" << std::endl << "    paz-archive"
            " [directory] (output path - default: [directory].paz)" << std::endl
            << std::endl;
        return 1;
    }
    else
    {
        files = std::unordered_set<std::string>(argv + 1, argv + argc - 1);
    }

    // Compress input files.
    paz::Archive archive;
    for(const auto& n : files)
    {
        archive.add(n, paz::load_bytes(isDir ? dirPath + "/" + n : n));
    }

    // Create archive.
    const std::string outPath = (isDir && argc < 3) ? dirPath + ".paz" : argv[
        argc - 1];
    archive.write(outPath);
}
