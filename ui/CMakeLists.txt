cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
find_package(Vulkan REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
add_compile_options(-g)
set(CMAKE_AUTOMOC ON)
add_library(UI STATIC
    MainWindow.cpp
    QVulkanMainWindow.cpp

    MainWindow.h
    QVulkanMainWindow.h
)

target_link_libraries(UI PRIVATE 
    tiny_obj_loader
    Renderer
    Vulkan::Vulkan 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets
)
