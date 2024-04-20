project(generaty)

set( ${PROJECT_NAME}_HEADERS
        generaty.hpp
)

set( ${PROJECT_NAME}_SOURCES
        generaty.cpp
)

add_library(${PROJECT_NAME}_library
        ${${PROJECT_NAME}_HEADERS}
        ${${PROJECT_NAME}_SOURCES}
)

target_link_libraries(
        ${PROJECT_NAME}_library PRIVATE
        document_library
)