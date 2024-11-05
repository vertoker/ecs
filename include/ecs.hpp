#pragma once

#include "base.hpp"
#include "types.hpp"
#include "component_pool.hpp"

#include <atomic>
#include <vector>
#include <unordered_map>
#include <memory>
#include <queue>
#include <bitset>
#include <set>

namespace ecs {

    class IWorld {
        entity CreateEntity();
        void DestroyEntity(entity entity);

        template <typename TComponent>
        void AddComponent(entity entity, TComponent component);
        template <typename TComponent>
        void RemoveComponent(entity entity);
        template <typename TComponent>
        TComponent& GetComponent(entity entity);
        
        template <typename TComponent>
        void RegisterComponent();
        template <typename TComponent>
        void UnregisterComponent();
    };

    template <size_t ENTITIES_COUNT = STANDARD_ENTITIES, size_t COMPONENTS_COUNT = STANDARD_COMPONENTS>
    class World : public IWorld {
    public:
        using interface_component_pool = IComponentPool<ENTITIES_COUNT>;
        using entity_entry = std::bitset<ENTITIES_COUNT>;

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
        void RemoveComponent(entity entity) {
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
            auto interfacePool = std::static_pointer_cast<interface_component_pool>(pool);
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
        std::shared_ptr<ComponentPool<TComponent, ENTITIES_COUNT>> GetOrCreateComponentPool() {
            type_index componentType = TypeIndexator<TComponent>::value();

            if (componentPools.find(componentType) == componentPools.end()) {
                assert(componentPools.size() < COMPONENTS_COUNT && "Too many components in the world");
                auto pool = std::make_shared<ComponentPool<TComponent, ENTITIES_COUNT>>();
                auto interfacePool = std::static_pointer_cast<interface_component_pool>(pool);
                componentPools.insert_or_assign(componentType, interfacePool);
                return pool;
            }
            
            std::shared_ptr<interface_component_pool> componentPool = componentPools[componentType];
            return std::static_pointer_cast<ComponentPool<TComponent, ENTITIES_COUNT>>(componentPool);
        }

        template <typename TComponent>
        std::shared_ptr<ComponentPool<TComponent, ENTITIES_COUNT>> GetComponentPool() {
            type_index componentType = TypeIndexator<TComponent>::value();

            assert(componentPools.find(componentType) != componentPools.end() && "ComponentPool can't found, register component");
            
            std::shared_ptr<interface_component_pool> componentPool = componentPools[componentType];
            return std::static_pointer_cast<ComponentPool<TComponent, ENTITIES_COUNT>>(componentPool);
        }
        
    public: // Readonly Data



    private:
        std::array<entity_entry, ENTITIES_COUNT> signatures{};
        std::queue<entity> availableEntities{};
        uint32_t entitiesCount{};

        std::unordered_map<type_index, std::shared_ptr<interface_component_pool>> componentPools{};
    };

    class System {
    protected:
        std::set<entity> entities;
    };

    class Systems {
    public:
        Systems(IWorld& world) : world{world} {
            
        }

    public:
        template<typename TSystem>
        std::shared_ptr<TSystem> RegisterSystem() {
            static_assert(std::is_default_constructible_v<TSystem>, "System must contains default constructor");
            type_index systemType = TypeIndexator<TSystem>::value();

            assert(systems.find(systemType) != systems.end() && "Registering system more than once");

            auto system = std::make_shared<TSystem>();
            systems.insert_or_assign(systemType, system);
            return system;
        }
        template<typename TSystem>
        void RegisterSystem(std::shared_ptr<TSystem> system) {
            type_index systemType = TypeIndexator<TSystem>::value();

            assert(systems.find(systemType) != systems.end() && "Registering system more than once");

            systems.insert_or_assign(systemType, system);
        }

        template<typename TSystem>
        void UnregisterSystem() {
            static_assert(std::is_default_constructible_v<TSystem>, "System must contains default constructor");
            type_index systemType = TypeIndexator<TSystem>::value();

            assert(systems.find(systemType) == systems.end() && "Can't find system in systems");
            
            systems.erase(systemType);
        }
        
    private:
        IWorld world;
        std::unordered_map<type_index, std::shared_ptr<System>> systems{};
    };
}