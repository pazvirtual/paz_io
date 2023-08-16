#include "PAZ_IO"
#include <unordered_set>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

int main(int argc, char** argv)
{
    // Check for options.
    const std::string arg1 = argv[1];
    const bool convert = argc >= 2 && (arg1 == "-c" || arg1 == "--convert");

    // Print help and return if invalid arguments.
    if(argc < convert + 2)
    {
        std::cout << std::endl << "Usage:" << std::endl << "    paz-archive [op"
            "tions] <files> <output path>" << std::endl << "    paz-archive [op"
            "tions] <directory> [output path = <directory>.pazarchive]" << std::
            endl << std::endl << "Options: " << std::endl << "    -c/--convert:"
            " Convert each OBJ object into a PAZ model" << std::endl << std::
            endl;
        return 1;
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
    else
    {
        files = std::unordered_set<std::string>(argv + convert + 1, argv + argc
            - 1);
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
