cmake_minimum_required(VERSION 3.10)
project(Comic_Reader)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(demo_background src/demo_background.cpp)
target_link_libraries(demo_background PRIVATE sfml-graphics sfml-window sfml-system)

# Copy shaders into build directory so the demo can find them at runtime
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Optional ImGui demo
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/CMakeLists.txt AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml/CMakeLists.txt)
	add_subdirectory(external/imgui)
	add_subdirectory(external/imgui-sfml)

	add_executable(demo_imgui src/demo_imgui.cpp)
	target_link_libraries(demo_imgui PRIVATE sfml-graphics sfml-window sfml-system imgui-sfml)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
