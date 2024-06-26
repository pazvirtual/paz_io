#include "PAZ_IO"
#include <iostream>
#include <fstream>

#define CATCH \
    catch(const std::exception& e) \
    { \
        std::cerr << "Failed " << test << ": " << e.what() << std::endl; \
        return 1; \
    } \
    std::cout << "Passed " << test << std::endl; \
    ++test;

static const std::array<std::string, 2> TestPaths = {"a/b/c.d", ".a"};
static const std::array<std::array<std::string, 3>, 2> TestPathElems =
{{
    {"a/b", "c", "d"},
    {"", ".a", ""}
}};
static const std::string ArchivePath = "test-archive.paz";
static const std::string TestString = "Abcdefg.";
static const std::unordered_map<std::string, paz::Bytes> TestData =
{
    {"file1.ext", "content content\ncontent content"},
    {"file2.whatever", "other stuff"}
};
static const std::string PbmPath = "test-image.pbm";
static constexpr int ImgRes = 10;

int main(int, char** argv)
{
    int test = 1;

    // Split some paths.
    for(std::size_t i = 0; i < TestPaths.size(); ++i)
    {
        try
        {
            const auto elems = paz::split_path(TestPaths[i]);
            if(elems != TestPathElems[i])
            {
               throw std::runtime_error("Path elements do not match.");
            }
        }
        CATCH
    }

    // Split path to get working directory.
    std::string appDir;
    try
    {
        appDir = paz::split_path(argv[0])[0];
    }
    CATCH

    // Convert text to binary data, compress, decompress, and convert to text.
    try
    {
        if(paz::uncompress(paz::compress(TestString)).str() != TestString)
        {
            throw std::runtime_error("Strings do not match.");
        }
    }
    CATCH

    // Write an archive.
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

    // Load an archive.
    try
    {
        paz::Archive archive;
        archive.read(appDir + "/" + ArchivePath);
        const auto contents = archive.contents();
        if(archive.contents().size() != TestData.size())
        {
            throw std::runtime_error("Incorrect number of contents.");
        }
        for(const auto& n : contents)
        {
            if(!TestData.count(n))
            {
                throw std::runtime_error("Unrecognized name in contents list.");
            }
        }
        for(const auto& n : TestData)
        {
            if(archive.get(n.first) != n.second)
            {
                throw std::runtime_error("Bytes do not match.");
            }
        }
    }
    CATCH

    // Delete a file.
    try
    {
        paz::remove(appDir + "/" + ArchivePath);
    }
    CATCH

    // Check line ending conversion.
    try
    {
        paz::Bytes temp("\n\r0\r\n1\r2\r\r");
        if(temp.str() != "\n\n0\n1\n2\n\n")
        {
           throw std::runtime_error("Line ending conversion failed.");
        }
    }
    CATCH

    // Write, read, and delete a binary PBM file.
    try
    {
        paz::Image a(paz::ImageFormat::R8UNorm, ImgRes, ImgRes);
        for(int i = 0; i < ImgRes; ++i)
        {
            a.bytes()[ImgRes*i + i] = 255;
        }
        paz::write_bytes(appDir + "/" + PbmPath, paz::to_pbm(a));
        const paz::Image b = paz::parse_pbm(paz::read_bytes(appDir + "/" +
            PbmPath));
        if(a.bytes() != b.bytes())
        {
            throw std::runtime_error("Images do not match.");
        }
        paz::remove(appDir + "/" + PbmPath);
    }
    CATCH
}
