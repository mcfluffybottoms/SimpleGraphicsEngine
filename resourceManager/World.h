#ifndef WORLD
#define WORLD
#include <set>
#include <unordered_map>
#include "Entity.h"
#include "../resourceManager/Component.h"

using EntityMap = std::unordered_map<Entity, EntityStructure>;

class World {
public:
	World() = default;

	Entity generateEntityId() {
		return nextEntityId++;
	}

	Entity createEntity() {
		const Entity id = generateEntityId();
		entities.insert(id);
		return id;
	}

	template <typename T>
	bool entityHasComponent(Entity id) {
		if (!components.count(id)) return false;
		const auto& entity = components.at(id);
		if constexpr (std::is_same_v<T, RenderElement>) {
			return entity.hasRenderElement();
		}
		else if constexpr (std::is_same_v<T, TransformElement>) {
			return entity.hasTransformElement();
		}
		return false;
	}

	template <typename T>
	bool deleteComponent(Entity id) {
		if (!components.count(id)) return false;
		auto& entity = components[id];
		if constexpr (std::is_same_v<T, RenderElement>) {
			entity.add(std::shared_ptr<RenderElement>{});
			return true;
		}
		else if constexpr (std::is_same_v<T, TransformElement>) {
			entity.add(std::shared_ptr<TransformElement>{});
			return true;
		}
		return false;
	}

	template <typename T>
	void addComponent(Entity id, T&& component) {
		if (!components.count(id)) {
			components.emplace(id, EntityStructure());
		}
		auto& entity = components[id];
		entity.add(std::make_shared<T>(component));
	}

	template <typename T>
	void addComponent(T&& component) {
		auto id = createEntity();
		if (!components.count(id)) {
			components.emplace(id, EntityStructure());
		}
		auto& entity = components[id];
		entity.add(std::make_shared<T>(component));
	}

	auto& getComponent(Entity id) {
		return components.at(id);
	}

	std::set<Entity>& getAllEntities() {
		return entities;
	}

private:
	Entity nextEntityId = 0;
	std::set<Entity> entities;
	EntityMap components;
};

#endif // WORLD