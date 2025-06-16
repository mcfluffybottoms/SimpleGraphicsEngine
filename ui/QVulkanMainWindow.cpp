#include "QVulkanMainWindow.h"
#include "../renderer/QVulkanRenderer.h"

QVulkanMainWindow::QVulkanMainWindow(QWindow *parent, ResourceManager *resourceManager, World *world)
    : QVulkanWindow(parent), m_resourceManager(resourceManager), m_world(world)
{
}

QVulkanMainWindow::~QVulkanMainWindow()
{
	if (vulkanInstance() && vulkanInstance()->isValid()) {
        vulkanInstance()->destroy();
    }
}

QVulkanWindowRenderer* QVulkanMainWindow::createRenderer()
{
	return new QVulkanRenderer(this, std::move(m_resourceManager), std::move(m_world));
}
