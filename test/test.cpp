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
static const std::unordered_map<std::string, paz::Bytes> TestData =
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
        paz::Archive archive;
        for(const auto& n : TestData)
        {
            archive.add(n.first, n.second);
        }
        archive.write(appDir + "/" + ArchivePath);
    }
    CATCH

    // Test 4: Loading an archive.
    try
    {
        const paz::Archive archive(appDir + "/" + ArchivePath);
        for(const auto& n : TestData)
        {
            if(archive.get(n.first) != n.second)
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
