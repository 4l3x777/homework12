#include "file_splitter.h"

#include <fstream>

namespace MapReduce
{

std::vector<FileSplitter::BlockIndexer> FileSplitter::split(std::size_t threads_count) const
{
    std::ifstream::pos_type file_size = std::ifstream(get_filename(), std::ios::in | std::ios::ate).tellg();
    if (file_size == std::ifstream::pos_type(-1)) throw std::runtime_error("File not opened/found!");

    std::ptrdiff_t block_size = file_size / threads_count;
    std::vector<FileSplitter::BlockIndexer> blocks;
    std::ifstream::pos_type pos_begin = 0, pos_end;

    std::ifstream file(get_filename(), std::ios::in | std::ios_base::binary);

    for (std::size_t i = 0; i < threads_count; i++)
    {
        pos_end = (file_size - pos_begin < block_size ? file_size : pos_begin + block_size);
        file.seekg(pos_end);
        while (pos_end < file_size && file.peek() != '\n')
        {
            pos_end += 1;
            file.ignore(1);
        }
        blocks.push_back(FileSplitter::BlockIndexer(std::pair<std::ifstream::pos_type, std::ifstream::pos_type>(pos_begin, pos_end)));
        pos_begin = pos_end;
        if (pos_begin != file_size) pos_begin += 1;
        else break;
    }
    return blocks;
}

const std::string& FileSplitter::get_filename() const
{
    return m_filename;
}

}
