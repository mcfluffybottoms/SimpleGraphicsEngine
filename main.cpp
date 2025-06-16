#include <QApplication>
#include <QVulkanInstance>
#include <QDebug>
#include "resourceManager/Component.h"
#include "ui/MainWindow.h"
#include "ui/QVulkanMainWindow.h"
#include <QTextStream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QTextStream in(stdin);
    std::vector<std::string> meshPaths;

    std::cout << "Enter mesh paths (double enter to exit):" << std::endl;

    while (in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) {
            break;
        }
        meshPaths.push_back(line.toStdString());
    }

    if (meshPaths.empty()) {
        meshPaths.push_back("D:\\documents\\PersonalHell\\repos\\SimpleGraphicsEngine1\\cube.obj");
    }

    auto resourceManager = std::make_unique<ResourceManager>();
    auto world = std::make_unique<World>();
    

    for (const std::string& path : meshPaths) {
        auto mesh = resourceManager->getMesh(path);
        if (mesh) {
            int entityId = world->createEntity();
            world->addComponent(entityId, RenderElement(mesh));
            world->addComponent(entityId, TransformElement());
            qDebug() << "Loaded mesh:" << path;
        } else {
            qWarning() << "Failed to load mesh:" << path;
        }
    }

    auto instance = std::make_unique<QVulkanInstance>();
    if (!instance->create()) {
        qFatal("Failed to create Vulkan instance");
        return -1;
    }

     auto vulkanWindow = new QVulkanMainWindow(nullptr, resourceManager.get(), world.get());
    vulkanWindow->setVulkanInstance(instance.get());
    
    MainWindow mainWindow(vulkanWindow);
    mainWindow.show();
    
    vulkanWindow->setProperty("m_resourceManager", QVariant::fromValue(resourceManager.release()));
    vulkanWindow->setProperty("m_world", QVariant::fromValue(world.release()));
    vulkanWindow->setProperty("vulkanInstance", QVariant::fromValue(instance.release()));
    
    try {
        return app.exec();
    } catch (const std::exception& e) {
        qCritical() << "An error occurred:" << e.what();
        return -1;
    }
   
}