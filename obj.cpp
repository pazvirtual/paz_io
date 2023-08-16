#include "PAZ_IO"
#include <fstream>
#include <sstream>
#include <array>

std::unordered_map<std::string, std::vector<float>> paz::load_obj(const std::
    string& path)
{
    std::ifstream in(path);
    if(!in)
    {
        throw std::runtime_error("Unable to open \"" + path + "\".");
    }

    std::size_t n = 0;

    std::unordered_map<std::string, std::vector<float>> pos;

    std::string curName;
    std::vector<float> curPosData;
    std::string line;
    while(std::getline(in, line))
    {
        if(line.substr(0, 2) == "o ")
        {
            n += curPosData.size()/3;
            curName = line.substr(2);
            if(pos.count(curName))
            {
                throw std::runtime_error("Object \"" + curName + "\" already de"
                    "fined.");
            }
            curPosData.clear();
        }
        else if(line.substr(0, 2) == "v ")
        {
            std::stringstream ss(line.substr(2));
            for(int i = 0; i < 3; ++i)
            {
                std::string str;
                std::getline(ss, str, ' ');
                curPosData.push_back(std::stof(str));
            }
        }
        else if(line.substr(0, 2) == "f ")
        {
            std::stringstream ss(line.substr(2));
            std::string str;
            std::getline(ss, str, '/');
            std::array<std::size_t, 3> idx;
            idx[0] = std::stoull(str) - 1 - n;
            std::getline(ss, str, ' ');
            std::getline(ss, str, '/');
            idx[1] = std::stoull(str) - 1 - n;
            std::getline(ss, str, ' ');
            std::getline(ss, str, '/');
            idx[2] = std::stoull(str) - 1 - n;
            for(int i = 0; i < 3; ++i)
            {
                for(int j = 0; j < 3; ++j)
                {
                    pos[curName].push_back(curPosData[3*idx[i] + (j + 2)%3]);
                }
            }
        }
    }

    return pos;
}
