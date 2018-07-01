project(SoftwareRenderer)
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} kernel32.lib user32.lib gdi32.lib winmm.lib)
