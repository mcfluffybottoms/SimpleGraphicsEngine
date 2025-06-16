#include "ResourceManager.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

std::shared_ptr<Mesh> ResourceManager::getMesh(const std::string &source) {
    if (m_cache.count(source)) {
        return m_cache.at(source);
    }
    std::shared_ptr<Mesh> resource = loadMesh(source);
    m_cache.emplace(source, resource);
    return resource;
}

std::shared_ptr<Mesh> ResourceManager::loadMesh(const std::string &source) {
    auto mesh = std::make_shared<Mesh>();
    std::cout << "ResourceManager::loadMesh: " << "loading mesh" << std::endl;
    tinyobj::attrib_t attribute;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning, error;

    if (!tinyobj::LoadObj(&attribute, &shapes, &materials, &warning, &error, source.c_str())) {
        std::cout << "ResourceManager::loadMesh: " << warning + error << std::endl;
        return nullptr;
    }

    std::unordered_map<Node, uint32_t> uniqueNodes;

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            if (index.vertex_index < 0 || index.texcoord_index < 0)
                continue;

            Node node{};
            node.position = {
                attribute.vertices.at(3 * index.vertex_index + 0),
                attribute.vertices.at(3 * index.vertex_index + 1),
                attribute.vertices.at(3 * index.vertex_index + 2)
            };

            node.textureCoord = {
                attribute.texcoords.at(2 * index.texcoord_index + 0),
                1.0f - attribute.texcoords.at(2 * index.texcoord_index + 1)
            };

            if (uniqueNodes.count(node) == 0) {
                uniqueNodes[node] = static_cast<uint32_t>(mesh->nodes.size());
                mesh->nodes.push_back(node);
            }

            mesh->indices.push_back(uniqueNodes[node]);
        }
    }

    return mesh;
}