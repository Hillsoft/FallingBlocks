# CMakeList.txt : CMake project for FallingBlocks, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.9)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_STANDARD 20)

string(REPLACE "/Ob1" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /OPT:REF /OPT:ICF")

add_definitions(-DGLFW_INCLUDE_VULKAN)
add_definitions("-DRESOURCE_DIR=\"res\"")
add_definitions(-DNOMINMAX)

project ("FallingBlocks" CXX)

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
	COMMAND "glslc" "${SRCPATH}" -o "${DSTPATH}" -g
)
list(APPEND SHADER_OUTPUT "${DSTPATH}")
set(SHADER_OUTPUT "${SHADER_OUTPUT}" PARENT_SCOPE)
endfunction()

link_libraries(glfw)
link_libraries(Vulkan::Vulkan)

include_directories(src)

# Add source to this project's executable.
add_subdirectory(src)

add_executable(FallingBlocks "src/main.cpp" "app.rc")
target_link_libraries(FallingBlocks
	application
	globalsubsystemstack)

add_shader("vertex.glsl" "vertex.spv")
add_shader("fragment.glsl" "fragment.spv")
add_shader("colVertex.glsl" "colVertex.spv")
add_shader("colFragment.glsl" "colFragment.spv")
add_shader("fontVertex.glsl" "fontVertex.spv")
add_shader("fontFragment.glsl" "fontFragment.spv")
add_custom_target(shader_bytecode DEPENDS ${SHADER_OUTPUT})

set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_BINARY_DIR}/res")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/res/" COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/res/" "${CMAKE_CURRENT_BINARY_DIR}/res" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/res/" "${CMAKE_CURRENT_SOURCE_DIR}/res/localisation/")
add_custom_target(resources DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/res/")


set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_BINARY_DIR}/data")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/data/" COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data/" "${CMAKE_CURRENT_BINARY_DIR}/data" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/")
add_custom_target(datares DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/data/")

add_dependencies(FallingBlocks shader_bytecode resources datares)

# TODO: Add tests and install targets if needed.
