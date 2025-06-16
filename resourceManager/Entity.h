#ifndef ENTITY
#define ENTITY

#include <memory>
#include <cstdint>
#include "Component.h"

using Entity = uint32_t;

struct EntityStructure {
	std::shared_ptr<RenderElement> render;
	std::shared_ptr<TransformElement> transform;

	void add(std::shared_ptr<RenderElement>&& r) {
		render = std::move(r);
	}
	void add(std::shared_ptr<TransformElement>&& t) {
		transform = std::move(t);
	}
	void deleteEntity() {
		render.reset();
		transform.reset();
	}
	bool hasTransformElement() const {
		return transform.get();
	}
	bool hasRenderElement() const {
		return render.get();
	}
};


#endif // ENTITY