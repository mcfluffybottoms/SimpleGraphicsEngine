cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
find_package(Vulkan REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-g)
add_library(ResourceManager STATIC
    ResourceManager.cpp
    Component.h
    Entity.h
    Resource.h
    ResourceManager.h
    World.h
)

target_link_libraries(ResourceManager PRIVATE 
    tiny_obj_loader
    Vulkan::Vulkan 
    Qt6::Core 
    Qt6::Widgets
    Qt6::Gui 
    glm
)
