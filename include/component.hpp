#pragma once

#include "base.hpp"
#include "types.hpp"

#include <unordered_map>
#include <memory>

namespace ecs {
    class IComponentPool {
    public:
        virtual ~IComponentPool() = default;
    };

    template<typename TComponent>
    class ComponentPool : IComponentPool {
    public:
        ComponentPool(size_t reserveComponents = 32) {
            components = std::make_unique<std::vector<TComponent>>();
            components->reserve(reserveComponents);
        }

        void AddComponent(entity entity, TComponent component) {
            components->emplace_back(component);
        }
        void RemoveComponent() {
            
        }

    private:
        std::unique_ptr<std::vector<TComponent>> components;
    };

    class ComponentManager {
    public:
        ComponentManager(size_t reservePools = 4) {
            componentPools = std::make_unique<std::unordered_map<type_index, std::shared_ptr<IComponentPool>>>(reservePools);
            componentPools->reserve(reservePools);
        }

        template <typename TComponent>
        void AddComponent(entity entity, TComponent component) {
            type_index componentType = TypeIndexator<TComponent>::value();
            auto pool = GetComponentPool<TComponent>();
            pool->AddComponent(component);
        };

        template <typename TComponent>
        void RemoveComponent(entity entity) {
            type_index componentType = TypeIndexator<TComponent>::value();
            auto pool = GetComponentPool<TComponent>();
            pool->RemoveComponent();
        };

        template <typename TComponent>
        std::shared_ptr<ComponentPool<TComponent>> CreateComponentPool() {
        }

        template <typename TComponent>
        std::shared_ptr<ComponentPool<TComponent>> GetComponentPool() {
            type_index componentType = TypeIndexator<TComponent>::value();

            assert(componentPools->find(componentType) != componentPools->end() && "ComponentPool can't found");

            auto& pComponentPools = *componentPools;
            std::shared_ptr<IComponentPool> componentPool = pComponentPools[componentType];
            return std::static_pointer_cast<ComponentPool<TComponent>>(componentPool);
        }

    private:
        std::unique_ptr<std::unordered_map<type_index, std::shared_ptr<IComponentPool>>> componentPools;
    };
}
