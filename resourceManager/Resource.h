#ifndef RESOURCE
#define RESOURCE

#include <glm/glm.hpp>
#include <ostream>
#include <vector>
#include <array>
#include <vulkan/vulkan_core.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>


#ifndef GLM_ENABLE_EXPERIMENTAL
namespace std {
	template<>
    struct hash<glm::vec2> {
        std::size_t operator()(const glm::vec2& v) const noexcept {
            std::size_t h1 = std::hash<float>{}(v.x);
            std::size_t h2 = std::hash<float>{}(v.y);
            return h1 ^ (h2 << 1);
        }
    };

    template<>
    struct hash<glm::vec3> {
        std::size_t operator()(const glm::vec3& v) const noexcept {
            std::size_t h1 = std::hash<float>{}(v.x);
            std::size_t h2 = std::hash<float>{}(v.y);
            std::size_t h3 = std::hash<float>{}(v.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    template<>
    struct hash<glm::ivec2> {
        std::size_t operator()(const glm::ivec2& v) const noexcept {
            std::size_t h1 = std::hash<int>{}(v.x);
            std::size_t h2 = std::hash<int>{}(v.y);
            return h1 ^ (h2 << 1);
        }
    };
}
#endif

struct Resource {
    virtual ~Resource() = default;
};

struct Node {
	glm::vec3 position;
	glm::vec3 color = {1.0f, 1.0f, 1.0f};
	glm::vec2 textureCoord;
	bool operator==(const Node &other) const {
		return position == other.position && color == other.color &&
				textureCoord == other.textureCoord;
	}

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Node);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Node, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Node, color);

        return attributeDescriptions;
    }

    friend std::ostream& operator<<(std::ostream &os, const Node &node);
    friend std::istream& operator>>(std::istream &os, const Node &node);

    // void serialize(std::ostream &os) {
    //     os << *this;
    // }

    // void deserialize(std::istream &os) {
    //     os >> *this;
    // }
};

// std::ostream& operator<< (std::ostream &os, const Node &node)
// {
//     return os << node.position.x << " " << node.position.y << " " << node.position.z << " " << node.color.x << " " << node.color.y << " " << node.color.z << " " << node.textureCoord.x << " " << node.textureCoord.y;
// }
// std::istream& operator>> (std::istream &os, const Node &node)
// {
//     return os >> node.position.x >> node.position.y >> node.position.z >> node.color.x >> node.color.y >> node.color.z >> node.textureCoord.x >> node.textureCoord.y;
// }

template <> struct std::hash<Node> {
size_t operator()(Node const &vertex) const {
	return (
		(hash<glm::vec3>()(vertex.position) ^ 
		(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ 
		(hash<glm::vec2>()(vertex.textureCoord) << 1
		);
}
};

struct Mesh : public Resource {
	std::vector<Node> nodes;
	std::vector<uint32_t> indices;
	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
	VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
};

#endif // RESOURCE