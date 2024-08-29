#include "PAZ_IO"
#include <sstream>

void paz::parse_model(const Bytes& content, std::vector<float>& positions, std::
    vector<float>& uvs, std::vector<float>& normals, std::vector<unsigned int>&
    materials, std::vector<std::string>& materialNames, std::vector<std::
    string>& materialLibs, std::vector<unsigned int>& indices)
{
    std::istringstream iss(content.str());

    positions.clear();
    uvs.clear();
    normals.clear();
    materials.clear();
    materialNames.clear();
    materialLibs.clear();
    indices.clear();

    std::string line;

    std::getline(iss, line);
    const std::size_t numVerts = std::stoull(line);
    positions.resize(4*numVerts);
    uvs.resize(2*numVerts);
    normals.resize(4*numVerts);
    materials.resize(numVerts);

    std::getline(iss, line);
    {
        std::istringstream iss1(line);
        while(std::getline(iss1, line, ' '))
        {
            materialNames.push_back(line);
        }
    }

    std::getline(iss, line);
    {
        std::istringstream iss1(line);
        while(std::getline(iss1, line, ' '))
        {
            materialLibs.push_back(line);
        }
    }

    for(std::size_t i = 0; i < numVerts; ++i)
    {
        std::getline(iss, line);
        std::istringstream iss1(line);
        for(int j = 0; j < 3; ++j)
        {
            iss1 >> positions[4*i + j];
        }
        positions[4*i + 3] = 1.;
        for(int j = 0; j < 2; ++j)
        {
            iss1 >> uvs[2*i + j];
        }
        for(int j = 0; j < 3; ++j)
        {
            iss1 >> normals[4*i + j];
        }
        normals[4*i + 3] = 0.;
        iss1 >> materials[i];
    }

    while(std::getline(iss, line))
    {
        indices.push_back(std::stoull(line));
    }
}

paz::Bytes paz::to_model(const std::vector<float>& positions, const std::vector<
    float>& uvs, const std::vector<float>& normals, const std::vector<unsigned
    int>& materials, const std::vector<std::string>& materialNames, const std::
    vector<std::string>& materialLibs, const std::vector<unsigned int>& indices)
{
    std::ostringstream oss;
    oss << materials.size() << '\n';
    for(std::size_t i = 0; i < materialNames.size(); ++i)
    {
        oss << materialNames[i] << (i + 1 < materialNames.size() ? " " : "");
    }
    oss << '\n';
    for(std::size_t i = 0; i < materialLibs.size(); ++i)
    {
        oss << materialLibs[i] << (i + 1 < materialLibs.size() ? " " : "");
    }
    oss << '\n';
    for(std::size_t i = 0; i < materials.size(); ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            oss << positions[4*i + j] << ' ';
        }
        for(int j = 0; j < 2; ++j)
        {
            oss << uvs[2*i + j] << ' ';
        }
        for(int j = 0; j < 3; ++j)
        {
            oss << normals[4*i + j] << ' ';
        }
        oss << materials[i] << '\n';
    }
    for(auto n : indices)
    {
        oss << n << '\n';
    }
    return oss.str();
}
