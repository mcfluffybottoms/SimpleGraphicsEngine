#ifndef QVULKAN_WINDOW
#define QVULKAN_WINDOW

#include <QVulkanWindow>
#include <QWidget>
#include "../resourceManager/ResourceManager.h"
#include "../resourceManager/World.h"

class QVulkanMainWindow : public QVulkanWindow
{
public:
	QVulkanMainWindow(QWindow *parent, ResourceManager *resourceManager, World *world);
	~QVulkanMainWindow();

	QVulkanWindowRenderer* createRenderer() override;
private:
	std::unique_ptr<ResourceManager> m_resourceManager{};
	std::unique_ptr<World> m_world{};
};

#endif // QVULKAN_WINDOW
