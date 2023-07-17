#pragma once

#include "mapper.h"
#include "reducer.h"
#include "file_splitter.h"
#include "thread_pool.h"

#include <string>
#include <vector>

namespace MapReduce
{

class Handler
{
public:
    Handler(std::size_t map_threads_count, std::size_t reduce_threads_count);

    std::size_t mapreduce_procedure(const std::string& filename);

private:
    std::vector<std::string> map_procedure(
        std::shared_ptr<std::vector<FileSplitter::BlockIndexer>> blocks, 
        std::shared_ptr<Mapper> mapper, 
        std::shared_ptr<progschj::ThreadPool> map_thread_pool
    );

    std::vector<std::pair<std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator>> shuffle_procedure(const std::vector<std::string>& map_data);

    bool reduce_procedure(
        const std::vector<std::pair<std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator>>& blocks,
        const reduce_function& reduce_function,
        std::shared_ptr<progschj::ThreadPool> reduce_thread_pool
    );

    std::size_t m_map_threads_count;
    std::size_t m_reduce_threads_count;
};

}
