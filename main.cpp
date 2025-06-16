#include <QApplication>
#include <QVulkanInstance>
#include <QDebug>
#include "resourceManager/Component.h"
#include "ui/MainWindow.h"
#include "ui/QVulkanMainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    auto resourceManager = std::make_unique<ResourceManager>();
    resourceManager->getMesh("D:\\documents\\PersonalHell\\repos\\SimpleGraphicsEngine1\\cube.obj");
    auto mesh = resourceManager->getMesh("D:\\documents\\PersonalHell\\repos\\SimpleGraphicsEngine1\\cube.obj");
    auto world = std::make_unique<World>();
    auto instance = std::make_unique<QVulkanInstance>();
    
    if (!instance->create()) {
        qFatal("Failed to create Vulkan instance");
    }

    auto vulkanWindow = new QVulkanMainWindow(nullptr, resourceManager.get(), world.get());
    vulkanWindow->setVulkanInstance(instance.get());
    
    MainWindow mainWindow(vulkanWindow);
    mainWindow.show();
    
    vulkanWindow->setProperty("m_resourceManager", QVariant::fromValue(resourceManager.release()));
    vulkanWindow->setProperty("m_world", QVariant::fromValue(world.release()));
    vulkanWindow->setProperty("vulkanInstance", QVariant::fromValue(instance.release()));
    
    world->addComponent(RenderElement(mesh));
    world->addComponent(0, TransformElement());
    try{
        return app.exec();
    }catch(...) {
        throw;
    }
   
}