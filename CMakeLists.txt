cmake_minimum_required(VERSION 3.12)

project(SoftwareRenderer)

set(EXTERNAL_DIR {CMAKE_SOURCE_DIR}/src/external)

add_executable(white_noise src/samples/white_noise.cpp)
target_link_libraries(white_noise kernel32.lib user32.lib gdi32.lib winmm.lib)

add_executable(moving_rect src/samples/moving_rect.cpp)
target_link_libraries(moving_rect kernel32.lib user32.lib gdi32.lib winmm.lib)

add_executable(lines src/samples/lines.cpp)
target_link_libraries(lines kernel32.lib user32.lib gdi32.lib winmm.lib)

add_executable(test_geometry src/tests/test_geometry.cpp)
target_include_directories(test_geometry PRIVATE src/external)
