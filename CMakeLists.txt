cmake_minimum_required(VERSION 3.12)

project(SoftwareRenderer)
set (CMAKE_CXX_STANDARD 14)

set(OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIRECTORY})

file(GLOB COMMON_FILES src/common/*)
add_library(renderer ${COMMON_FILES})

list(APPEND LIBS
    renderer kernel32 user32 gdi32 winmm
)

file(GLOB SAMPLES src/samples/*.cpp)
foreach(SAMPLE ${SAMPLES})
    get_filename_component(NAME ${SAMPLE} NAME_WE)
    add_executable(${NAME} ${SAMPLE})
    target_link_libraries(${NAME} ${LIBS})
endforeach(SAMPLE)

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
