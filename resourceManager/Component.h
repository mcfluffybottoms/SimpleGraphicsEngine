#ifndef COMPONENTS
#define COMPONENTS

#include "Resource.h"
#include <memory>

class Component {
public:
    virtual ~Component() = 0;
};

inline Component::~Component() = default;

class RenderElement : public Component {
public:
    RenderElement(std::shared_ptr<Mesh> m) : mesh(m) {}
    ~RenderElement() override = default;
    
    std::shared_ptr<Mesh> mesh;
};

class TransformElement : public Component {
public:
    ~TransformElement() override = default;
    
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
};

#endif // COMPONENTS