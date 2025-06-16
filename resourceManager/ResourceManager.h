#ifndef RESOURCEMANAGER
#define RESOURCEMANAGER

#include "Resource.h"
#include <iostream>
#include <memory>
#include <string>
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

using Cache = std::unordered_map<std::string, std::shared_ptr<Mesh>>;

class ResourceManager {
public:
  ResourceManager() {}
  std::shared_ptr<Mesh> getMesh(const std::string &source);

private:
  Cache m_cache;

  std::shared_ptr<Mesh> loadMesh(const std::string &source);
};

#endif // RESOURCEMANAGER