#include <filesystem>
#include <fstream>
#include "QVulkanRenderer.h"
#include <vulkan/vulkan_core.h>

QVulkanRenderer::QVulkanRenderer(
    QVulkanWindow *parent, std::unique_ptr<ResourceManager> &&resourceManager,
    std::unique_ptr<World> &&world)
    : m_window(parent), m_resourceManager(std::move(resourceManager)),
      m_world(std::move(world)) {}

QVulkanRenderer::~QVulkanRenderer() {}

void QVulkanRenderer::initResources() {
  m_device = m_window->device();
  m_physicalDevice = m_window->physicalDevice();
  m_deviceFunctions = m_window->vulkanInstance()->deviceFunctions(m_device);

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = m_window->graphicsQueueFamilyIndex();

  if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) !=
      VK_SUCCESS) {
    throw std::runtime_error(
        "QVulkanRenderer::initResources(): Failed to create command pool.");
  }

  m_graphicsQueue = m_window->graphicsQueue();
  createPipelineLayout();
  createGraphicsPipeline();
}

std::vector<char> QVulkanRenderer::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << "\n";
        std::cerr << "Current working directory: " << std::filesystem::current_path() << "\n";
        throw std::runtime_error("failed to open file: " + filename);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

VkShaderModule QVulkanRenderer::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    
    VkShaderModule shaderModule;
    if (m_deviceFunctions->vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module.");
    }
    return shaderModule;
}


void QVulkanRenderer::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(glm::mat4);
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (m_deviceFunctions->vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout.");
    }
}

void QVulkanRenderer::createGraphicsPipeline() {
    auto fragShaderCode = readFile("frag.spv");
    auto vertShaderCode = readFile("vert.spv");
    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Node::getBindingDescription();
    auto attributeDescriptions = Node::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;

    if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline.");
    }

    vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
}

uint32_t QVulkanRenderer::findMemoryType(uint32_t typeFilter, 
                                        VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if (typeFilter & (1 << i)) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type.");
}


void QVulkanRenderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                  VkMemoryPropertyFlags properties,
                                  VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
    auto df = m_window->vulkanInstance()->deviceFunctions(m_device);
    if (!df) throw std::runtime_error("Device functions not available");

    if (size == 0) {
        buffer = VK_NULL_HANDLE;
        bufferMemory = VK_NULL_HANDLE;
        return;
    }

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (df->vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer");
    }

    VkMemoryRequirements memRequirements;
    df->vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    
    try {
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                                 properties);
    } catch (const std::exception& e) {
        std::cerr << "Memory type requirements:\n";
        std::cerr << " - Size: " << memRequirements.size << "\n";
        std::cerr << " - Alignment: " << memRequirements.alignment << "\n";
        std::cerr << " - Memory type bits: " << memRequirements.memoryTypeBits << "\n";
        std::cerr << " - Properties requested: " << properties << "\n";
        throw;
    }

    if (df->vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        std::stringstream ss;
        ss << "Failed to allocate buffer memory (size: " 
           << memRequirements.size << " bytes, type: " 
           << allocInfo.memoryTypeIndex << ")";
        throw std::runtime_error(ss.str());
    }

    df->vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
}

void QVulkanRenderer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                                 VkDeviceSize size) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = m_commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  if (m_deviceFunctions->vkAllocateCommandBuffers(
          m_device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate command buffer.");
  }

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  if (m_deviceFunctions->vkBeginCommandBuffer(commandBuffer, &beginInfo) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to begin command buffer");
  }

  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  m_deviceFunctions->vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1,
                                     &copyRegion);

  if (m_deviceFunctions->vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Failed to end command buffer.");
  }

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(m_graphicsQueue);

  m_deviceFunctions->vkFreeCommandBuffers(m_device, m_commandPool, 1,
                                          &commandBuffer);
}

void QVulkanRenderer::createMeshBuffers(Mesh &mesh) {
  VkDeviceSize vertexBufferSize = sizeof(Node) * mesh.nodes.size();
  VkDeviceSize indexBufferSize = sizeof(uint32_t) * mesh.indices.size();
  if (mesh.nodes.empty()) {
        qWarning() << "Mesh has no vertices!";
        mesh.vertexBuffer = VK_NULL_HANDLE;
        mesh.vertexBufferMemory = VK_NULL_HANDLE;
    }
    
    if (mesh.indices.empty()) {
        qWarning() << "Mesh has no indices!";
        mesh.indexBuffer = VK_NULL_HANDLE;
        mesh.indexBufferMemory = VK_NULL_HANDLE;
    }
  VkBuffer vertexStagingBuffer;
  VkDeviceMemory vertexStagingBufferMemory;
  createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               vertexStagingBuffer, vertexStagingBufferMemory);

  void *data;
  m_deviceFunctions->vkMapMemory(m_device, vertexStagingBufferMemory, 0,
                                 vertexBufferSize, 0, &data);
  memcpy(data, mesh.nodes.data(), static_cast<size_t>(vertexBufferSize));
  m_deviceFunctions->vkUnmapMemory(m_device, vertexStagingBufferMemory);

  createBuffer(vertexBufferSize,
               VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mesh.vertexBuffer,
               mesh.vertexBufferMemory);

  copyBuffer(vertexStagingBuffer, mesh.vertexBuffer, vertexBufferSize);

  VkBuffer indexStagingBuffer;
  VkDeviceMemory indexStagingBufferMemory;
  createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               indexStagingBuffer, indexStagingBufferMemory);

  m_deviceFunctions->vkMapMemory(m_device, indexStagingBufferMemory, 0,
                                 indexBufferSize, 0, &data);
  memcpy(data, mesh.indices.data(), static_cast<size_t>(indexBufferSize));
  m_deviceFunctions->vkUnmapMemory(m_device, indexStagingBufferMemory);

  createBuffer(indexBufferSize,
               VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                   VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mesh.indexBuffer,
               mesh.indexBufferMemory);

  copyBuffer(indexStagingBuffer, mesh.indexBuffer, indexBufferSize);

  m_deviceFunctions->vkDestroyBuffer(m_device, vertexStagingBuffer, nullptr);
  m_deviceFunctions->vkFreeMemory(m_device, vertexStagingBufferMemory, nullptr);
  m_deviceFunctions->vkDestroyBuffer(m_device, indexStagingBuffer, nullptr);
  m_deviceFunctions->vkFreeMemory(m_device, indexStagingBufferMemory, nullptr);
}

void QVulkanRenderer::initSwapChainResources() {}

void QVulkanRenderer::releaseSwapChainResources() {}

void QVulkanRenderer::releaseResources() {
    if (m_graphicsPipeline) {
        m_deviceFunctions->vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
        m_graphicsPipeline = VK_NULL_HANDLE;
    }
    
    if (m_pipelineLayout) {
        m_deviceFunctions->vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }
    
    if (m_commandPool) {
        m_deviceFunctions->vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
    }
}

void QVulkanRenderer::destroyMeshBuffers(Mesh& mesh) {
    
    if (mesh.vertexBuffer) m_deviceFunctions->vkDestroyBuffer(m_device, mesh.vertexBuffer, nullptr);
    if (mesh.vertexBufferMemory) m_deviceFunctions->vkFreeMemory(m_device, mesh.vertexBufferMemory, nullptr);
    if (mesh.indexBuffer) m_deviceFunctions->vkDestroyBuffer(m_device, mesh.indexBuffer, nullptr);
    if (mesh.indexBufferMemory) m_deviceFunctions->vkFreeMemory(m_device, mesh.indexBufferMemory, nullptr);
    
    mesh.vertexBuffer = VK_NULL_HANDLE;
    mesh.vertexBufferMemory = VK_NULL_HANDLE;
    mesh.indexBuffer = VK_NULL_HANDLE;
    mesh.indexBufferMemory = VK_NULL_HANDLE;
}

void QVulkanRenderer::startNextFrame() {
  m_deviceFunctions = m_window->vulkanInstance()->deviceFunctions(m_device);
  if (!m_deviceFunctions) {
    qWarning("Device functions not available!");
    return;
  }

  VkCommandBuffer cmdBuf = m_window->currentCommandBuffer();
  VkRenderPass renderPass = m_window->defaultRenderPass();
  VkFramebuffer framebuffer = m_window->currentFramebuffer();
  QSize sz = m_window->swapChainImageSize();

  VkClearValue clearValues[2] = {
      {{0.0f, 0.0f, 0.0f, 1.0f}},
      {1.0f, 0}
  };
  
  VkRenderPassBeginInfo rpBeginInfo{};
  rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  rpBeginInfo.renderPass = renderPass;
  rpBeginInfo.framebuffer = framebuffer;
  rpBeginInfo.renderArea = {{0, 0},
                            {static_cast<uint32_t>(sz.width()), 
                             static_cast<uint32_t>(sz.height())}};
  rpBeginInfo.clearValueCount = 2;
  rpBeginInfo.pClearValues = clearValues;

  m_deviceFunctions->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
  
  m_deviceFunctions->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(sz.width());
  viewport.height = static_cast<float>(sz.height());
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = {static_cast<uint32_t>(sz.width()), 
                   static_cast<uint32_t>(sz.height())};

  m_deviceFunctions->vkCmdSetViewport(cmdBuf, 0, 1, &viewport);
  m_deviceFunctions->vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

  glm::mat4 model = glm::mat4(1.0f);
  m_deviceFunctions->vkCmdPushConstants(
      cmdBuf, 
      m_pipelineLayout, 
      VK_SHADER_STAGE_VERTEX_BIT, 
      0, 
      sizeof(glm::mat4), 
      &model
  );

  for (Entity e : m_world->getAllEntities()) {
    if (!m_world->entityHasComponent<RenderElement>(e) ||
        !m_world->entityHasComponent<TransformElement>(e)) {
      continue;
    }

    auto [render, transform] = m_world->getComponent(e);

    if (render && render->mesh) {
      auto mesh = render->mesh;
      if(!mesh) continue;

      // if (mesh && mesh->nodes.empty() || mesh->indices.empty()) {
      //   qDebug() << "Skipping empty mesh for entity" << e;
      //   continue;
      // }
      
      if (!mesh->vertexBuffer || !mesh->indexBuffer) {
        createMeshBuffers(*mesh);
      }
      
      VkBuffer vertexBuffers[] = {mesh->vertexBuffer};
      VkDeviceSize offsets[] = {0};
      m_deviceFunctions->vkCmdBindVertexBuffers(cmdBuf, 0, 1, vertexBuffers, offsets);
      m_deviceFunctions->vkCmdBindIndexBuffer(cmdBuf, mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
      
      m_deviceFunctions->vkCmdDrawIndexed(
          cmdBuf, 
          static_cast<uint32_t>(mesh->indices.size()), 
          1, 
          0, 
          0, 
          0
      );
    }
  }

  m_deviceFunctions->vkCmdEndRenderPass(cmdBuf);
  m_window->frameReady();
  m_window->requestUpdate();
}
