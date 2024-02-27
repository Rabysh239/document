cmake_minimum_required(VERSION 3.27)
project(document)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup()

file(GLOB_RECURSE SIMDJSON "include/simdjson/*.h")
file(GLOB_RECURSE SIMDJSON_DOM "include/simdjson/dom/*.h")
file(GLOB_RECURSE SIMDJSON_GENERIC "include/simdjson/generic/*.h")
file(GLOB_RECURSE SIMDJSON_INTERNAL "include/simdjson/internal/*.h")
file(GLOB_RECURSE SRC "src/*.h")
file(GLOB_RECURSE SRC_GENERIC "src/generic/*.h")
file(GLOB_RECURSE SRC_GENERIC_STAGE2 "src/generic/stage2/*.h")
file(GLOB_RECURSE SRC_INTERNAL "src/internal/*.cpp")

add_executable(document ${SIMDJSON} ${SIMDJSON_DOM} ${SIMDJSON_GENERIC} ${SIMDJSON_INTERNAL} ${SRC} ${SRC_GENERIC} ${SRC_GENERIC_STAGE2} ${SRC_INTERNAL} main.cpp)

target_link_libraries(document ${CONAN_LIBS})
