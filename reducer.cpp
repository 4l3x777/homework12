#include "reducer.h"

namespace MapReduce
{

bool reducer(reducer_iterator begin, reducer_iterator end)
{
    if (begin == end) return true;
    for (auto it = std::next(begin); it != end; ++it) 
    {
        if (*std::prev(it) == *it) 
        {
            return false;
        }
    }
    return true;
}

}
