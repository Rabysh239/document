cmake_minimum_required(VERSION 3.27)
project(document)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include(FetchContent)

conan_basic_setup()

file(GLOB_RECURSE DOCUMENT_HPP "include/components/document/*.hpp")
file(GLOB_RECURSE DOCUMENT_CPP "include/components/document/*.cpp")
file(GLOB_RECURSE SIMDJSON "include/simdjson/*.h")
file(GLOB_RECURSE SIMDJSON_DOM "include/simdjson/dom/*.h")
file(GLOB_RECURSE SIMDJSON_GENERIC "include/simdjson/generic/*.h")
file(GLOB_RECURSE SIMDJSON_INTERNAL "include/simdjson/internal/*.h")
file(GLOB_RECURSE SRC "src/*.h")
file(GLOB_RECURSE SRC_GENERIC "src/generic/*.h")
file(GLOB_RECURSE SRC_GENERIC_STAGE2 "src/generic/stage2/*.h")
file(GLOB_RECURSE SRC_INTERNAL "src/internal/*.cpp")

add_executable(document ${DOCUMENT_HPP} ${DOCUMENT_CPP} ${SIMDJSON} ${SIMDJSON_DOM} ${SIMDJSON_GENERIC} ${SIMDJSON_INTERNAL} ${SRC} ${SRC_GENERIC} ${SRC_GENERIC_STAGE2} ${SRC_INTERNAL} main.cpp)

FetchContent_Declare(
        radix_trie
        GIT_REPOSITORY https://github.com/ytakano/radix_tree.git
        GIT_TAG f8bc846ef1f00ee43af2ddc04af67d2e44d50685
)
FetchContent_MakeAvailable(radix_trie)
FetchContent_GetProperties(radix_trie)
target_include_directories(document PRIVATE ${radix_trie_SOURCE_DIR})

target_link_libraries(document ${CONAN_LIBS})
