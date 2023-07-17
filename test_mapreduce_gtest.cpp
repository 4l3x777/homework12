#include "gtest/gtest.h"
#include "handler.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

class TestMapReduce : public ::testing::TestWithParam<std::string>
{
	std::string filename;
protected:
	void SetUp() {}
	void TearDown() {}
};

std::vector<std::string> read_file(const std::string& filename)
{
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string tmp;
    while (std::getline(file, tmp))
        lines.emplace_back(std::move(tmp));
    return lines;
}

std::size_t get_correct_answer(const std::string& filename)
{
    auto lines = read_file(filename);
    std::size_t prefix_length = 1;
    std::vector<std::string> prefixes;
    for (;; ++prefix_length)
    {
        for (auto line: lines)
        {
            if (line.size() == 1 && line[0] == 0x0) continue;
            else if (line.size() < prefix_length) 
            {
                return std::size_t(-1);
            }
            prefixes.push_back(line.substr(0, prefix_length));
        }
        std::sort(prefixes.begin(), prefixes.end());
        auto prefixes_end = std::unique(prefixes.begin(), prefixes.end());
        if (prefixes_end == prefixes.end()) break;
        prefixes.clear();
    }
    return prefix_length;
}

TEST_P(TestMapReduce, test_mapreduce)
{	
	const std::string& filename = GetParam();
    std::size_t correct = get_correct_answer(filename);
    for (std::size_t mnum = 1; mnum < std::thread::hardware_concurrency() / 2; ++mnum)
    {
        for (std::size_t rnum = 1; rnum < std::thread::hardware_concurrency() / 2; ++rnum)
        {
            MapReduce::Handler handler(mnum, rnum);
            std::cout << correct << std::endl;
            EXPECT_EQ(correct, handler.mapreduce_procedure(filename));
        }
    }
}

INSTANTIATE_TEST_CASE_P
(
    MapReduce,
    TestMapReduce,
    ::testing::Values
    (
#if defined(WIN32)
		"..\\test\\corpus1.txt",
        "..\\test\\corpus2.txt"
#else
		"test/corpus1.txt",
		"test/corpus2.txt"
#endif
    )
);

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
