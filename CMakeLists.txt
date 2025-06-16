cmake_minimum_required(VERSION 3.8)
project(SimpleGraphicsEngine)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O2")
find_package(Vulkan REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/glm)
add_library(tiny_obj_loader INTERFACE)
target_include_directories(tiny_obj_loader INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/tinyobjloader)

add_subdirectory(ResourceManager)
add_subdirectory(Renderer)
add_subdirectory(UI)

add_executable(engine_main main.cpp)

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