cmake_minimum_required(VERSION 3.27)
project(document)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

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

add_compile_options(
        -Wall
        -Wextra
        -Wpedantic

        -Wcast-align
        -Wcast-qual
        -Wconversion
        -Wctor-dtor-privacy
        -Wenum-compare
        -Wfloat-equal
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wredundant-decls
        -Wsign-conversion
        -Wsign-promo
)
add_compile_options(-fno-omit-frame-pointer)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(document ${DOCUMENT_HPP} ${DOCUMENT_CPP} ${SIMDJSON} ${SIMDJSON_DOM} ${SIMDJSON_GENERIC} ${SIMDJSON_INTERNAL} ${SRC} ${SRC_GENERIC} ${SRC_GENERIC_STAGE2} ${SRC_INTERNAL} main.cpp)

target_link_libraries(document ${CONAN_LIBS})
