#include "PAZ_IO"
#include <unordered_set>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

static const std::string HelpMsg = 1 + R"===(
Usage:
    paz-archive [options] <files> <output path>
    paz-archive [options] <directory> [output path = <directory>.pazarchive]

Options:
    -c/--convert: Convert each OBJ object into a PAZ model
)===";

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cout << std::endl << HelpMsg << std::endl;
        return 1;
    }

    // Check for options.
    bool convert = false;
    if(argc >= 2)
    {
        const std::string arg1 = argv[1];
        convert = arg1 == "-c" || arg1 == "--convert";
    }

    // Figure out if this is a directory.
    std::unordered_set<std::string> files;
    struct stat s;
    const bool isDir = (argc == convert + 2 || argc == convert + 3) && !stat(
        argv[convert + 1], &s) && S_ISDIR(s.st_mode);
    std::string dirPath;
    if(isDir)
    {
        // Extract bare directory path (no trailing slash) for later.
        dirPath = argv[convert + 1];
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
        if((dir = opendir(argv[convert + 1])))
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
                argv[convert + 1] << "\"." << std::endl << std::endl;
            return 1;
        }
    }
    else if(argc > convert + 2)
    {
        files = std::unordered_set<std::string>(argv + convert + 1, argv + argc
            - 1);
    }
    else
    {
        std::cout << std::endl << HelpMsg << std::endl;
        return 1;
    }

    // Compress input files.
    paz::Archive archive;
    for(const auto& n : files)
    {
        auto contents = paz::read_bytes(isDir ? dirPath + "/" + n : n);
        if(convert && n.size() > 4 && n.substr(n.size() - 4) == ".obj")
        {
            std::vector<std::string> names;
            std::vector<std::vector<float>> positions;
            std::vector<std::vector<float>> uvs;
            std::vector<std::vector<float>> normals;
            std::vector<std::vector<unsigned int>> materials;
            std::vector<std::string> materialNames;
            std::vector<std::string> materialLibs;
            std::vector<std::vector<unsigned int>> indices;
            paz::parse_obj(contents, names, positions, uvs, normals, materials,
                materialNames, materialLibs, indices);
            for(std::size_t i = 0; i < names.size(); ++i)
            {
                const std::string path = n.substr(0, n.size() - 4) + (names.
                    size() > 1 ? "_" + names[i] : "") + ".pazmodel";
                std::cout << "Adding \"" << path << "\"" << std::endl;
                archive.add(path, paz::to_model(positions[i], uvs[i], normals[
                    i], materials[i], materialNames, materialLibs, indices[i]));
            }
        }
        else
        {
            std::cout << "Adding \"" << n << "\"" << std::endl;
            archive.add(n, contents);
        }
    }

    // Create archive.
    const std::string outPath = (isDir && argc < convert + 3) ? dirPath +
        ".pazarchive" : argv[argc - 1];
    archive.write(outPath);
}
