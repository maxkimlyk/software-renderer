cmake_minimum_required(VERSION 3.12)

project(SoftwareRenderer)
set (CMAKE_CXX_STANDARD 14)

set(OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIRECTORY})
    list(APPEND LIBS kernel32 user32 gdi32 winmm)
endif (WIN32)

if (UNIX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
    list(APPEND LIBS X11)
    file(GLOB PLATFORM_SRC src/platform/linux/*)
endif (UNIX)

file(GLOB RENDERER_SRC src/renderer/*)
add_library(renderer ${RENDERER_SRC} ${PLATFORM_SRC})

add_library(tgaimage src/external/tgaimage/tgaimage.cpp)

list(APPEND LIBS renderer tgaimage)
list(APPEND LIBS renderer)

file(GLOB EXAMPLES src/examples/*.cpp)

foreach(EXAMPLE ${EXAMPLES})
    get_filename_component(NAME ${EXAMPLE} NAME_WE)
    add_executable(${NAME} ${EXAMPLE})
    target_link_libraries(${NAME} ${LIBS})
endforeach(EXAMPLE)

file(GLOB TESTS src/tests/*.cpp)
foreach(TEST ${TESTS})
    get_filename_component(NAME ${TEST} NAME_WE)
    add_executable(${NAME} ${TEST})
    target_include_directories(${NAME} PRIVATE src/external)
    target_link_libraries(${NAME} ${LIBS})
endforeach(TEST)

add_custom_target(data_files
        ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data ${OUTPUT_DIRECTORY}/)

add_dependencies(model data_files)
add_dependencies(scene data_files)
