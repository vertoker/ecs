#pragma once

#include "base.hpp"
#include "component.hpp"

#include <atomic>
#include <vector>
#include <unordered_map>

namespace ecs {
    class EntityManager {
    public:
        static const size_t STANDARD_RESERVE_ENTITIES = 5000;
        static const size_t STANDARD_RESERVE_COMPONENTS = 10;

        EntityManager(size_t reserveEntities = STANDARD_RESERVE_ENTITIES) {
            entities.reserve(reserveEntities);
        }

        entity CreateEntity(size_t reserveComponents = STANDARD_RESERVE_COMPONENTS) {
            static std::atomic<entity> counter{};
            entity newEntity = counter++;

            std::vector<type_index> newComponents{};
            newComponents.reserve(reserveComponents);

            entities.emplace(newEntity, newComponents);
            return newEntity;
        }

        void DestroyEntity(entity entity) {
            entities.erase(entity);
        }

    private:
        std::unordered_map<entity, std::vector<type_index>> entities{};
    };
}
