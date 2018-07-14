cmake_minimum_required(VERSION 3.12)

project(SoftwareRenderer)

set(OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIRECTORY})

add_executable(white_noise src/samples/white_noise.cpp)
target_link_libraries(white_noise kernel32.lib user32.lib gdi32.lib winmm.lib)

add_executable(moving_rect src/samples/moving_rect.cpp)
target_link_libraries(moving_rect kernel32.lib user32.lib gdi32.lib winmm.lib)

add_executable(lines src/samples/lines.cpp)
target_link_libraries(lines kernel32.lib user32.lib gdi32.lib winmm.lib)

add_executable(triangles src/samples/triangles.cpp)
target_link_libraries(triangles kernel32.lib user32.lib gdi32.lib winmm.lib)

add_executable(test_geometry src/tests/test_geometry.cpp)
target_include_directories(test_geometry PRIVATE src/external)

add_executable(test_model src/tests/test_model.cpp)

add_custom_command(TARGET test_model POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data ${OUTPUT_DIRECTORY}/)
