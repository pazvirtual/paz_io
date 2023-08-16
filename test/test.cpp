#include "PAZ_IO"
#include <fstream>

#define CATCH \
    catch(const std::exception& e) \
    { \
        std::cerr << "Failed " << test << ": " << e.what() << std::endl; \
        return 1; \
    } \
    std::cout << "Passed " << test << std::endl; \
    ++test;


static const std::string ArchivePath = "test-archive.paz";
static const std::string TestString = "Abcdefg.";
static const std::unordered_map<std::string, std::string> TestData =
{
    {"file1.ext", "content content\ncontent content"},
    {"file2.whatever", "other stuff"}
};

int main(int, char** argv)
{
    int test = 1;

    // Test 1: Splitting path to get working directory.
    std::string appDir;
    try
    {
        appDir = paz::split_path(argv[0])[0];
    }
    CATCH

    // Test 2: Compressing and decompressing text.
    std::string res;
    try
    {
        if(paz::uncompress(paz::compress(TestString)).str() != TestString)
        {
            throw std::runtime_error("Strings do not match.");
        }
    }
    CATCH

    // Test 3: Writing an archive.
    try
    {
        std::unordered_map<std::string, paz::Bytes> data;
        for(const auto& n : TestData)
        {
            data[n.first] = paz::compress(n.second);
        }
        paz::write_archive(appDir + "/" + ArchivePath, data);
    }
    CATCH

    // Test 4: Loading an archive's contents list.
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
    CATCH

    // Test 5: Loading data from an archive.
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
    CATCH

    // Test 6: Deleting a file.
    try
    {
        paz::remove(appDir + "/" + ArchivePath);
    }
    CATCH
}
