#pragma once

#include "entities.hpp"
#include "components.hpp"
#include "systems.hpp"

#include <atomic>
#include <vector>
#include <unordered_map>
#include <memory>
#include <queue>
#include <bitset>

namespace ecs {

    template <size_t ENTITIES_COUNT = STANDARD_ENTITIES, size_t COMPONENTS_COUNT = STANDARD_COMPONENTS>
    class World {
    public:
        World() {
            for (entity entity = 0; entity < ENTITIES_COUNT; entity++) {
			    availableEntities.push(entity);
            }
        }
        
    public: // Entities

        entity CreateEntity() {
            assert(entitiesCount < ENTITIES_COUNT && "Too many active entities in world");

            entity newEntity = availableEntities.front();
            availableEntities.pop();
            entitiesCount++;
            
            return newEntity;
        }
        
        void DestroyEntity(entity entity) {
            assert(entity < ENTITIES_COUNT && "Entity out of world range (too big)");
            assert(entity >= 0 && "Entity out of world range (is negative)");

            signatures[entity].reset();

            availableEntities.push(entity);
            entitiesCount--;
        }

    public: // Components

        template <typename TComponent>
        void AddComponent(entity entity, TComponent component) {
            auto pool = GetComponentPool<TComponent>();
            pool->InsertComponent(entity, component);
        };
        
        template <typename TComponent>
        void RemoveComponent(entity entity, component_address componentAddress) {
            auto pool = GetComponentPool<TComponent>();
            pool->RemoveComponent();
        };
        
        template <typename TComponent>
        TComponent& GetComponent(entity entity) {
            auto pool = GetComponentPool<TComponent>();
            return pool->GetComponent();
        };
        
    public: // Component Pools

        template <typename TComponent>
        void RegisterComponent() {
            assert(componentPools.size() < COMPONENTS_COUNT && "Too many components in the world");

            type_index componentType = TypeIndexator<TComponent>::value();
            
            assert(!componentPools.contains(componentType) && "Component already registered");

            auto pool = std::make_shared<ComponentPool<TComponent, ENTITIES_COUNT>>();
            auto interfacePool = std::static_pointer_cast<IComponentPool<ENTITIES_COUNT>>(pool);
            componentPools.insert_or_assign(componentType, interfacePool);
        };

        template <typename TComponent>
        void UnregisterComponent() {
            type_index componentType = TypeIndexator<TComponent>::value();

            assert(componentPools.contains(componentType) && "Component doesn't registered");

            componentPools.erase(componentType);
        };

    private:
        template <typename TComponent>
        std::shared_ptr<ComponentPool<TComponent, ENTITIES_COUNT>> GetComponentPool() {
            type_index componentType = TypeIndexator<TComponent>::value();

            assert(componentPools.find(componentType) != componentPools.end() && "ComponentPool can't found, register component pool");
            
            std::shared_ptr<IComponentPool<ENTITIES_COUNT>> componentPool = componentPools[componentType];
            return std::static_pointer_cast<ComponentPool<TComponent, ENTITIES_COUNT>>(componentPool);
        }

    private:
        std::array<std::bitset<COMPONENTS_COUNT>, ENTITIES_COUNT> signatures{};
        std::queue<entity> availableEntities{};
        uint32_t entitiesCount{};

        std::unordered_map<type_index, std::shared_ptr<IComponentPool<ENTITIES_COUNT>>> componentPools{};
        
        SystemManager systemManager{};
    };
}