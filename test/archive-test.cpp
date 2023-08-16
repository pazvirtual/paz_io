#include "PAZ_IO"
#include <fstream>

static const std::string ArchivePath = "test-archive.paz";
static const std::string TestString = "Abcdefg.";
static const std::unordered_map<std::string, std::string> TestData =
{
    {"file1.ext", "content content\ncontent content"},
    {"file2.whatever", "other stuff"}
};

int main(int, char** argv)
{
    const std::string appDir = paz::split_path(argv[0])[0];

    // Test 1: Compressing and decompressing text.
    std::string res;
    try
    {
        if(paz::uncompress(paz::compress(TestString)) != TestString)
        {
            throw std::runtime_error("Strings do not match.");
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Failed 1: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Passed 1" << std::endl;

    // Test 2: Writing an archive.
    try
    {
        std::unordered_map<std::string, std::vector<unsigned char>> data;
        for(const auto& n : TestData)
        {
            data[n.first] = paz::compress(n.second);
        }
        paz::write_archive(appDir + "/" + ArchivePath, data);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Failed 2: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Passed 2" << std::endl;

    // Test 3: Loading an archive's contents list.
    try
    {
        const std::unordered_map<std::string, std::size_t> contents = paz::
            load_contents_list(appDir + "/" + ArchivePath);
        if(contents.size() != TestData.size())
        {
            throw std::runtime_error("Number of blocks does not match.");
        }
        for(const auto& n : contents)
        {
            if(!TestData.count(n.first))
            {
                throw std::runtime_error("Block names do not match.");
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Failed 3: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Passed 3" << std::endl;

    // Test 4: Loading data from an archive.
    try
    {
        for(const auto& n : TestData)
        {
            const std::string data = paz::load_block(appDir + "/" + ArchivePath,
                n.first);
            if(data != n.second)
            {
                throw std::runtime_error("Strings do not match.");
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Failed 4: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Passed 4" << std::endl;
}
