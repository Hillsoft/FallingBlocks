# CMakeList.txt : CMake project for Tetris, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_STANDARD 20)

add_definitions(-DGLFW_INCLUDE_VULKAN)

project ("Tetris" CXX)

find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(GTest CONFIG REQUIRED)

enable_testing()
include(GoogleTest)

function(add_gtest name)
add_executable("${name}" "${ARGN}")
target_link_libraries("${name}" PRIVATE GTest::gtest GTest::gtest_main)
add_test("${name}" "${name}")
# gtest_discover_tests("${name}")
endfunction()

set(SHADER_OUTPUT "")
function(add_shader SRCNAME DSTNAME)
set(SRCPATH "${CMAKE_SOURCE_DIR}/src/shaders/${SRCNAME}")
set(DSTPATH "${CMAKE_CURRENT_BINARY_DIR}/shaders/${DSTNAME}")
add_custom_command(
	OUTPUT "${DSTPATH}"
	MAIN_DEPENDENCY "${SRCPATH}"
	COMMAND "glslc" "${SRCPATH}" -o "${DSTPATH}"
)
list(APPEND SHADER_OUTPUT "${DSTPATH}")
set(SHADER_OUTPUT "${SHADER_OUTPUT}" PARENT_SCOPE)
endfunction()

link_libraries(glfw)
link_libraries(Vulkan::Vulkan)

include_directories(src)

# Add source to this project's executable.
add_subdirectory(src)

add_executable(FallingBlocks "src/main.cpp")
target_link_libraries(FallingBlocks
	render.glfwapplication)

add_shader("vertex.glsl" "vertex.spv")
add_shader("fragment.glsl" "fragment.spv")
add_custom_target(shader_bytecode DEPENDS ${SHADER_OUTPUT})
add_dependencies(FallingBlocks shader_bytecode)

# TODO: Add tests and install targets if needed.
