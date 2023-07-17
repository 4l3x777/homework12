#include "mapper.h"

#include <fstream>
#include <sstream>

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace MapReduce
{

Mapper::Mapper(std::string filename, std::size_t prefix_length)
    : m_filename(filename)
    , m_prefix_length(prefix_length)
{
}

std::string Mapper::read_file_block(const FileSplitter::BlockIndexer& block_idx) const
{
    std::ifstream file;
    file.open(get_filename(), std::ios::in | std::ios_base::binary);
    file.seekg(file.beg + block_idx.first);
    std::string block_data(block_idx.second - block_idx.first + 1, 0x0);
    file.read(block_data.data(), block_idx.second - block_idx.first);
    file.close();
    return block_data;
}

std::list<std::string> Mapper::operator()(const FileSplitter::BlockIndexer& block_idx) const
{
    std::string block_data = read_file_block(block_idx);
    std::list<std::string> result;

    std::vector<std::string> lines;
    boost::split(lines, block_data, boost::is_any_of("\t\n "), boost::token_compress_on);

    for (auto line: lines)
    {
        if (line.size() == 1 && line[0] == 0x0) continue;
        else if (m_prefix_length > line.size()) 
        {
            throw PrefixException{};
        }
        result.emplace_back(line.substr(0, m_prefix_length));
    }
    result.sort();
    return result;
}

const std::string& Mapper::get_filename() const
{
    return m_filename;
}

}
