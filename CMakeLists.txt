cmake_minimum_required(VERSION 3.8)
project(SimpleGraphicsEngine)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    message(STATUS "Build type set to Debug (default)")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # For tooling support
set(CMAKE_CXX_STANDARD 20)
# Enable debug symbols and disable optimizations in Debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
# Keep debug symbols in Release builds for crash analysis
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O2")
find_package(Vulkan REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Third-party includes
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/glm)

add_library(tiny_obj_loader INTERFACE)
target_include_directories(tiny_obj_loader INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/tinyobjloader)

# Subprojects
add_subdirectory(ResourceManager)
add_subdirectory(Renderer)
add_subdirectory(UI)

# Main executable
add_executable(engine_main main.cpp)

# Link dependencies
target_link_libraries(engine_main PRIVATE
    tiny_obj_loader
    glm
    UI
    Renderer
    ResourceManager
    Vulkan::Vulkan
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Enable debug features in Qt
target_compile_definitions(engine_main PRIVATE $<$<CONFIG:Debug>:QT_QML_DEBUG>)