#pragma once

#include "file_splitter.h"

#include <list>
#include <string>
#include <stdexcept>

namespace MapReduce
{

class Mapper
{
public:
    struct PrefixException : public std::exception {};

    Mapper(std::string filename, std::size_t prefix_length);

    std::list<std::string> operator()(const FileSplitter::BlockIndexer& block_idx) const;

    const std::string& get_filename() const;

private:
    std::string read_file_block(const FileSplitter::BlockIndexer& block_idx) const;
    std::string m_filename;
    std::ifstream m_file;
    std::size_t m_prefix_length;
};

}
