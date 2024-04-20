cmake_minimum_required(VERSION 3.27)
project(document)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup(TARGETS)

file(GLOB_RECURSE DOCUMENT_SOURCES "src/components/document/*.cpp")
file(GLOB_RECURSE INTERNAL_SOURCES "src/internal/*.cpp")

add_compile_options(-g)
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
#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)

add_library(
        document_library
        ${DOCUMENT_SOURCES}
        ${INTERNAL_SOURCES}
)

target_include_directories(document_library PUBLIC "${CMAKE_SOURCE_DIR}/include/")
target_include_directories(document_library PUBLIC "${CMAKE_SOURCE_DIR}/src/")

target_link_libraries(
        document_library
        CONAN_PKG::boost
        CONAN_PKG::abseil
)

add_subdirectory(components)
add_subdirectory(benchmark)
add_subdirectory(test)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        document_library
)
