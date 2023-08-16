#include "PAZ_IO"
#include <fstream>
#include <sstream>
#include <numeric>
#include <algorithm>

static std::size_t find_vertex(float x, float y, float z, std::vector<float>& v,
    std::size_t start)
{
    const std::size_t numVertices = v.size()/4;
    for(std::size_t i = start; i < numVertices; ++i)
    {
        if(v[4*i] == x && v[4*i + 1] == y && v[4*i + 2] == z)
        {
            return i;
        }
    }
    return v.size();
}

void paz::load_obj(const std::string& path, std::vector<std::string>& names,
    std::vector<std::vector<float>>& positions, std::vector<std::vector<float>>&
    uvs, std::vector<std::vector<float>>& normals, std::vector<std::vector<
    unsigned int>>& materials, std::vector<std::string>& materialNames, std::
    vector<std::string>& materialLibs)
{
    std::ifstream in(path);
    if(!in)
    {
        throw std::runtime_error("Unable to open \"" + path + "\".");
    }

    names.clear();
    positions.clear();
    uvs.clear();
    normals.clear();
    materials.clear();
    materialNames.clear();
    materialLibs.clear();

    // Parse file.
    std::vector<float> curPositions;
    std::vector<float> curUvs;
    std::vector<float> curNormals;
    std::string curMaterial;
    unsigned int curMtlIdx = 0u;
    std::size_t numPositions = 0;
    std::size_t numUvs = 0;
    std::size_t numNormals = 0;
    std::string line;
    while(std::getline(in, line))
    {
        if(line.substr(0, 7) == "mtllib ")
        {
            const std::string n = line.substr(7);
            if(std::find(materialLibs.begin(), materialLibs.end(), n) ==
                materialLibs.end())
            {
                materialLibs.push_back(n);
            }
        }
        else if(line.substr(0, 2) == "o ")
        {
            const std::string n = line.substr(2);
            if(std::find(names.begin(), names.end(), n) != names.end())
            {
                throw std::runtime_error("Object \"" + n + "\" already defined."
                    );
            }
            numPositions += curPositions.size()/3;
            numUvs += curUvs.size()/2;
            numNormals += curNormals.size()/3;
            curPositions.clear();
            curUvs.clear();
            curNormals.clear();
            curMaterial = "";
            curMtlIdx = 0u;
            names.push_back(n);
            positions.emplace_back();
            uvs.emplace_back();
            normals.emplace_back();
            materials.emplace_back();
        }
        else if(line.substr(0, 2) == "v ")
        {
            std::stringstream ss(line.substr(2));
            for(std::size_t i = 0; i < 3; ++i)
            {
                double n;
                ss >> n;
                curPositions.push_back(n);
            }
        }
        else if(line.substr(0, 3) == "vt ")
        {
            std::stringstream ss(line.substr(3));
            for(int i = 0; i < 3; ++i)
            {
                double n;
                ss >> n;
                curUvs.push_back(n);
            }
        }
        else if(line.substr(0, 3) == "vn ")
        {
            std::stringstream ss(line.substr(3));
            for(int i = 0; i < 3; ++i)
            {
                double n;
                ss >> n;
                curNormals.push_back(n);
            }
        }
        else if(line.substr(0, 7) == "usemtl ")
        {
            curMaterial = line.substr(7);
            curMtlIdx = std::find(materialNames.begin(), materialNames.end(),
                curMaterial) - materialNames.begin() + 1u;
            if(curMtlIdx > materialNames.size())
            {
                materialNames.push_back(curMaterial);
            }
        }
        else if(line.substr(0, 2) == "f ")
        {
            std::istringstream iss(line.substr(2));

            std::array<std::size_t, 3> posIdx = {};
            std::array<std::size_t, 3> uvIdx = {};
            std::array<std::size_t, 3> norIdx = {};
            for(std::size_t i = 0; i < 3; ++i)
            {
                std::getline(iss, line, '/');
                if(!line.empty())
                {
                    posIdx[i] = std::stoull(line) - 1 - numPositions;
                }
                std::getline(iss, line, '/');
                if(!line.empty())
                {
                    uvIdx[i] = std::stoull(line) - 1 - numUvs;
                }
                std::getline(iss, line, ' ');
                if(!line.empty())
                {
                    norIdx[i] = std::stoull(line) - 1 - numNormals;
                }
            }

            for(std::size_t i = 0; i < 3; ++i)
            {
                for(std::size_t j = 0; j < 3; ++j)
                {
                    const std::size_t p = 3*posIdx[i] + (j + 2)%3;
                    if(p < curPositions.size())
                    {
                        positions.back().push_back(curPositions[p]);
                    }
                    else
                    {
                        positions.back().push_back(0.f);
                    }

                    const std::size_t n = 3*norIdx[i] + (j + 2)%3;
                    if(n < curNormals.size())
                    {
                        normals.back().push_back(curNormals[n]);
                    }
                    else
                    {
                        normals.back().push_back(0.f);
                    }
                }
                positions.back().push_back(1.f);
                normals.back().push_back(0.f);
                for(std::size_t j = 0; j < 2; ++j)
                {
                    const std::size_t u = 2*uvIdx[i] + j;
                    if(u < curUvs.size())
                    {
                        uvs.back().push_back(curUvs[u]);
                    }
                    else
                    {
                        uvs.back().push_back(0.f);
                    }
                }
                materials.back().push_back(curMtlIdx);
            }
        }
    }
}

void paz::load_obj(const std::string& path, std::vector<std::string>& names,
    std::vector<std::vector<float>>& positions, std::vector<std::vector<float>>&
    uvs, std::vector<std::vector<float>>& normals, std::vector<std::vector<
    unsigned int>>& materials, std::vector<std::string>& materialNames, std::
    vector<std::string>& materialLibs, std::vector<std::vector<unsigned int>>&
    indices)
{
    load_obj(path, names, positions, uvs, normals, materials, materialNames,
        materialLibs);

    // Remove duplicates and set indices.
    const std::size_t numObjects = names.size();
    indices.resize(numObjects);
    for(std::size_t i = 0; i < numObjects; ++i)
    {
        std::size_t numVertices = positions[i].size()/4;
        indices[i].resize(numVertices);
        std::iota(indices[i].begin(), indices[i].end(), 0u);
        for(std::size_t j = 0; j < numVertices; ++j)
        {
            std::size_t k = j + 1;
            while(true)
            {
                k = find_vertex(positions[i][4*j], positions[i][4*j + 1],
                    positions[i][4*j + 2], positions[i], k);
                if(k >= numVertices)
                {
                    break;
                }
                else if(uvs[i][2*k] == uvs[i][2*j] && uvs[i][2*k + 1] == uvs[i][
                    2*j + 1] && normals[i][4*k] == normals[i][4*j] && normals[
                    i][4*k + 1] == normals[i][4*j + 1] && normals[i][4*k + 2] ==
                    normals[i][4*j + 2] && materials[i][k] == materials[i][j])
                {
                    positions[i].erase(positions[i].begin() + 4*k, positions[i].
                        begin() + 4*(k + 1));
                    uvs[i].erase(uvs[i].begin() + 2*k, uvs[i].begin() + 2*(k +
                        1));
                    normals[i].erase(normals[i].begin() + 4*k, normals[i].
                        begin() + 4*(k + 1));
                    materials[i].erase(materials[i].begin() + k);
                    --numVertices;
                    for(auto& n : indices[i])
                    {
                        if(n == k)
                        {
                            n = j;
                        }
                        else if(n > k)
                        {
                            --n;
                        }
                    }
                }
                else
                {
                    ++k;
                }
            }
        }
    }
}
