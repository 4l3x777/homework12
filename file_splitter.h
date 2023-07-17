#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace MapReduce
{
    
class FileSplitter
{
    std::string m_filename;
public:
    using BlockIndexer = std::pair<std::ifstream::pos_type, std::ifstream::pos_type>;

    FileSplitter(std::string filename) : m_filename(filename) {}
    const std::string& get_filename() const;
    std::vector<BlockIndexer> split(std::size_t threads_count) const;
};

}
