#include "handler.h"

#include <boost/program_options.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    try 
    {
        boost::program_options::options_description named_description 
        {
            "Allowed options"
        };

        named_description.add_options()
        ("help,h", "give this help list")
        ("src,s",
                boost::program_options::value<std::string>()->value_name("FILE_PATH")->default_value(std::string("example.txt")),
                "mapreduce file path")
        ("mnum,m",
            boost::program_options::value<size_t>()->value_name("COUNT")->default_value(1),
            "number of map threads"
        )
        ("rnum,r",
            boost::program_options::value<size_t>()->value_name("COUNT")->default_value(1),
            "number of reduce threads"
        );

        boost::program_options::options_description cmdline_description { };
        cmdline_description.add(named_description);

        boost::program_options::variables_map variables;
        boost::program_options::store(
        boost::program_options::command_line_parser(argc, argv)
            .options(cmdline_description)
            .run(),
        variables);
        boost::program_options::notify(variables);

        if (variables.count("help")) 
        {
            std::cout << named_description << std::endl;
            return false;
        }

        std::size_t mnum = variables["mnum"].as<std::size_t>();
        if (mnum <= 0)
        {
            std::cerr << "Number of map threads must be at least 1" << std::endl;
            return 1;
        }

        std::size_t rnum = variables["rnum"].as<std::size_t>();
        if (rnum <= 0)
        {
            std::cerr << "Number of reduce threads  must be at least 1" << std::endl;
            return 1;
        }

        std::string src = variables["src"].as<std::string>();

        MapReduce::Handler handler(mnum, rnum);
        auto result = handler.mapreduce_procedure(src);
        if (result == std::size_t(-1))
            std::cout << "Cannot detect minimum prefix length" << std::endl;
        else
            std::cout << "Minimum prefix length is " << result << std::endl;
    }
    catch (boost::program_options::error const &e) 
    {
        std::cerr << "Options error: " << e.what() << std::endl;
        return false;
    }

    return 0;
}
