#ifndef QVULKAN_RENDERER
#define QVULKAN_RENDERER

#include "../resourceManager/ResourceManager.h"
#include "../resourceManager/World.h"
#include <QVulkanDeviceFunctions>
#include <QVulkanWindowRenderer>
#include <memory>

class QVulkanRenderer : public QVulkanWindowRenderer {
public:
  QVulkanRenderer(QVulkanWindow *parent,
                  std::unique_ptr<ResourceManager> &&resourceManager,
                  std::unique_ptr<World> &&world);
  ~QVulkanRenderer() override;

  void initResources() override;
  void initSwapChainResources() override;
  void releaseSwapChainResources() override;
  void releaseResources() override;
  void startNextFrame() override;

  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
  void uploadDataToBuffer(VkBuffer buffer, void *data, VkDeviceSize size, VkDeviceMemory& bufferMemory);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);
  void createMeshBuffers(Mesh &mesh);

  VkShaderModule createShaderModule(const std::vector<char>& code);
  std::vector<char> readFile(const std::string& filename);
  void createPipelineLayout();
  void createGraphicsPipeline();

  void destroyMeshBuffers(Mesh& mesh);

private:
  QVulkanWindow *m_window{};
  std::unique_ptr<ResourceManager> m_resourceManager{};
  std::unique_ptr<World> m_world{};
  QVulkanDeviceFunctions *m_deviceFunctions = VK_NULL_HANDLE;
  VkDevice m_device = VK_NULL_HANDLE;
  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkCommandPool m_commandPool = VK_NULL_HANDLE;
  VkQueue m_graphicsQueue = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
  VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
  VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
};

#endif // QVULKAN_RENDERER
