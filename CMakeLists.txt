project(SoftwareRenderer)

add_executable(white_noise src/samples/white_noise.cpp)
target_link_libraries(white_noise kernel32.lib user32.lib gdi32.lib winmm.lib)

add_executable(moving_rect src/samples/moving_rect.cpp)
target_link_libraries(moving_rect kernel32.lib user32.lib gdi32.lib winmm.lib)
