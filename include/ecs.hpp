#pragma once

#include "entities.hpp"
#include "components.hpp"
#include "systems.hpp"

#include <atomic>
#include <vector>
#include <unordered_map>
#include <memory>

namespace ecs {

    // TODO: refactor to the original ECS, every entity can have only one type definition

    class World {
    public:
        World(size_t reservedEntities = RESERVED_ENTITIES, size_t reservePools = RESERVED_POOLS) {
            entities.reserve(reservedEntities);
            componentPools.reserve(reservePools);
        }

        // Entities

    public:
        entity CreateEntity() {
            static std::atomic<entity> counter{};
            entity newEntity = counter++;

            std::unordered_set<component_address> components{};
            components.reserve(RESERVED_ENTITY_COMPONENTS);
            entities.emplace(newEntity, components);

            return newEntity;
        }
        
        void DestroyEntity(entity entity) {
            entities.erase(entity);
        }

        // Components

    public:
        template <typename TComponent>
        void AddComponent(entity entity, TComponent component) {
            type_index componentType = TypeIndexator<TComponent>::value();
            auto pool = GetComponentPool<TComponent>();
            auto componentIndex = pool->AddComponent(component);

            component_address componentAddress(componentType, componentIndex);
            entities[entity].emplace(componentAddress);
        };
        
        template <typename TComponent>
        void RemoveComponent(entity entity, component_address componentAddress) {
            type_index componentType = TypeIndexator<TComponent>::value();
            auto pool = GetComponentPool<TComponent>();
            pool->RemoveComponent();


        };

        // Component Pools

    public:
        template <typename TComponent>
        void RegisterComponent() {
            type_index componentType = TypeIndexator<TComponent>::value();
            auto pool = std::make_shared<ComponentPool<TComponent>>();
            auto iPool = std::static_pointer_cast<IComponentPool>(pool);
            
            componentPools.insert_or_assign(componentType, iPool);
        };

        template <typename TComponent>
        void UnregisterComponent() {
            type_index componentType = TypeIndexator<TComponent>::value();
            componentPools.erase(componentType);
        };

    private:
        template <typename TComponent>
        std::shared_ptr<ComponentPool<TComponent>> GetComponentPool() {
            type_index componentType = TypeIndexator<TComponent>::value();

            assert(componentPools.find(componentType) != componentPools.end() && "ComponentPool can't found");
            
            std::shared_ptr<IComponentPool> componentPool = componentPools[componentType];
            return std::static_pointer_cast<ComponentPool<TComponent>>(componentPool);
        }

    private:
        std::unordered_map<entity, std::unordered_set<component_address>> entities{};
        std::unordered_map<type_index, std::shared_ptr<IComponentPool>> componentPools{};
        
        SystemManager systemManager{};
    };
}