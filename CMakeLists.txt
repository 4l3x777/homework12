cmake_minimum_required(VERSION 3.25)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(mapreduce VERSION ${PROJECT_VESRION})

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.13.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

set(BOOST_INCLUDE_LIBRARIES program_options algorithm)
set(BOOST_ENABLE_CMAKE ON)
#Windows help debug
#set(Boost_DEBUG ON)
FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost)

add_executable(
    mapreduce 
    file_splitter.cpp
    handler.cpp
    mapper.cpp
    reducer.cpp
    thread_pool.cpp
    main.cpp
)

add_executable(
    test_mapreduce_gtest
    file_splitter.cpp
    handler.cpp
    mapper.cpp
    reducer.cpp
    thread_pool.cpp
    test_mapreduce_gtest.cpp
)

set_target_properties(
    mapreduce
    test_mapreduce_gtest
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(
    test_mapreduce_gtest 
    PROPERTIES 
    COMPILE_DEFINITIONS 
    STATIC_LINK
)

target_link_libraries(
    test_mapreduce_gtest    
    gtest 
    gtest_main
    Boost::program_options
    Boost::algorithm
)

target_link_libraries(
    mapreduce
    Boost::program_options
    Boost::algorithm
)

if (MSVC)
    target_compile_options(mapreduce PRIVATE /W4)
    target_compile_options(test_mapreduce_gtest PRIVATE /W4)
else ()
    target_link_options(mapreduce PRIVATE -static -static-libgcc -static-libstdc++)
    target_link_options(test_mapreduce_gtest PRIVATE -static -static-libgcc -static-libstdc++)

    target_compile_options(
        mapreduce 
        PRIVATE
        -Wall 
        -Wextra 
        -pedantic 
    )
    target_compile_options(
        test_mapreduce_gtest 
        PRIVATE
        -Wall 
        -Wextra 
        -pedantic 
    )
endif()

install (
    TARGETS 
    mapreduce 
    RUNTIME 
    DESTINATION 
    bin
)

# CPack area
if (WIN32)
    set(CPACK_GENERATOR "ZIP")
else ()
    set(CPACK_GENERATOR "DEB")
endif()
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT https://github.com/4l3x777/homework1)
include(CPack)

# Tests area
enable_testing()

add_test(
    NAME 
    unit_tests_test_mapreduce_gtest 
    COMMAND 
    test_mapreduce_gtest
)
