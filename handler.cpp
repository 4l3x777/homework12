#include "handler.h"

#include <cassert>
#include <future>
#include <iterator>
#include <list>
#include <vector>
#include <memory>


namespace MapReduce
{

Handler::Handler(std::size_t map_threads_count, std::size_t reduce_threads_count) : m_map_threads_count(map_threads_count), m_reduce_threads_count(reduce_threads_count) {}

std::vector<std::string> Handler::map_procedure(std::shared_ptr<std::vector<FileSplitter::BlockIndexer>> blocks, std::shared_ptr<Mapper> mapper, std::shared_ptr<progschj::ThreadPool> map_thread_pool)
{
    // map blocks
    using map_result = std::future<std::list<std::string>>;
    auto jobs_count = blocks->size() < m_map_threads_count ? blocks->size() : m_map_threads_count;
    std::vector<map_result> map_jobs(jobs_count);
    for (std::size_t i = 0; i < jobs_count; ++i)
    {
        auto map_job = [i, &blocks, &mapper]()
        {
            return (*mapper)((*blocks)[i]);
        };
        map_jobs[i] = map_thread_pool->enqueue(map_job);
    }

    std::list<std::string> tmp;
    bool prefix_exception_flag = false;
    for (map_result& map_result : map_jobs)
    {
        try
        {
            tmp.merge(map_result.get());
        }
        catch (const Mapper::PrefixException&)
        {
            prefix_exception_flag = true;
        }
    }
    if (prefix_exception_flag) throw Mapper::PrefixException{};

    return std::vector<std::string>(std::move_iterator(tmp.begin()), std::move_iterator(tmp.end()));
}

std::vector<std::pair<std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator>> Handler::shuffle_procedure(const std::vector<std::string>& map_data)
{
    // shuffle map_data
    auto jobs_count = map_data.size() < m_reduce_threads_count ? map_data.size() : m_reduce_threads_count;
    using It = std::vector<std::string>::const_iterator;
    auto blocks = std::vector<std::pair<It, It>>(jobs_count);
    std::size_t block_size = map_data.size() / jobs_count;
    It block_begin = map_data.begin(), block_end;
    for (std::size_t i = 0; i < jobs_count; i++)
    {
        block_end = (map_data.end() - block_begin < block_size ? map_data.end() : block_begin + block_size);
        if (block_end == map_data.end())
        {
            blocks[i].first = block_begin;
            blocks[i].second = block_end;
            block_begin = block_end;
            continue;
        }
        while (std::next(block_end) != map_data.end() && *std::next(block_end) == *block_end) ++block_end;
        blocks[i].first = block_begin;
        blocks[i].second = std::next(block_end);
        block_begin = std::next(block_end);
    }

    return blocks;
}

bool Handler::reduce_procedure(
    const std::vector<std::pair<std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator>>& blocks,
    const reduce_function& reduce_function,
    std::shared_ptr<progschj::ThreadPool> reduce_thread_pool
)
{
    // reduce blocks
    std::vector<std::future<bool>> reduce_results_collector(m_reduce_threads_count);
    for (auto i = 0; i < m_reduce_threads_count; ++i)
    {
        auto reducer_job = [reduce_function, i, &blocks]()
        {
            return reduce_function(blocks[i].first, blocks[i].second);
        };
        reduce_results_collector[i] = reduce_thread_pool->enqueue(reducer_job);
    }

    bool answer = true;
    for (std::future<bool>& future_result : reduce_results_collector)
    {
        if (!future_result.get()) answer = false;
    }
    return answer;
}

std::size_t Handler::mapreduce_procedure(const std::string& filename)
{
    auto blocks = std::make_shared<std::vector<FileSplitter::BlockIndexer>>(FileSplitter(filename).split(m_map_threads_count));
    // start prefix length
    std::size_t prefix_length = 1;
    std::vector<std::string> map_data;
    try
    {
        auto map_thread_pool = std::make_shared<progschj::ThreadPool>(m_map_threads_count);
        auto reduce_thread_pool = std::make_shared<progschj::ThreadPool>(m_reduce_threads_count);
        for (;; ++prefix_length)
        {
            {
                auto mapper = std::make_shared<Mapper>(filename, prefix_length);
                map_data = map_procedure(blocks, mapper, map_thread_pool);
            }
            if (reduce_procedure(shuffle_procedure(map_data), &reducer, reduce_thread_pool))
                break;
        }
    }
    catch (const Mapper::PrefixException&)
    {
        return std::size_t(-1);
    }
    return prefix_length;
}

}