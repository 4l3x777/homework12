#pragma once

#include <functional>
#include <string>
#include <vector>

namespace MapReduce
{

using reducer_iterator = std::vector<std::string>::const_iterator;
using reduce_function = std::function<bool (reducer_iterator begin, reducer_iterator end)>;

bool reducer(reducer_iterator begin, reducer_iterator end);

}
