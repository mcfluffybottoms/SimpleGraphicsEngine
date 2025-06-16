cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
find_package(Vulkan REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
# Enable debug symbols and disable optimizations in Debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
# Keep debug symbols in Release builds for crash analysis
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O2")
set(CMAKE_AUTOMOC ON)
add_library(Renderer STATIC
    QVulkanRenderer.cpp
    QVulkanRenderer.h
)

target_link_libraries(Renderer PRIVATE 
    tiny_obj_loader
    Vulkan::Vulkan 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets 
    glm
)
