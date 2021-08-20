cmake_minimum_required(VERSION 3.20)
project(LeetCode)

set(CMAKE_CXX_STANDARD 11)
aux_source_directory(. srcs)

foreach(target ${srcs})
    string(REGEX REPLACE ".\/" "" out ${target})
    string(REGEX REPLACE ".cpp" "" out ${out})
    add_executable(${out} ${target})
    if(UNIX)
        target_link_libraries(${out} pthread)
    endif()
endforeach(target)

