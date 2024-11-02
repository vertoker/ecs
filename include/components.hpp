#pragma once

#include "base.hpp"
#include "types.hpp"

namespace ecs {

    // Interface Component Pool

    class IComponentPool {
    public:
        virtual ~IComponentPool() = default;
    };

    // Component Pool

    template<typename TComponent>
    class ComponentPool : public IComponentPool {
    public:
        ComponentPool(size_t reserveComponents = RESERVED_COMPONENT_POOL) {
            components.reserve(reserveComponents);
        }

        component_index AddComponent(TComponent component) {
            component_index newComponentIndex = componentCounter++;
            components.emplace(newComponentIndex, component);

            return newComponentIndex;
        }

        void RemoveComponent(component_index componentIndex) {
            TComponent component = components[componentIndex];
            components.erase(componentIndex);
        }

    private:
        std::atomic<component_index> componentCounter{};
        std::unordered_map<component_index, TComponent> components{};
    };
}
