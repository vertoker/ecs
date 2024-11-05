#pragma once

#include "base.hpp"
#include "types.hpp"

#include <type_traits>

namespace ecs {

    // Interface Component Pool

    template<size_t ENTITIES_COUNT = STANDARD_ENTITIES>
    class IComponentPool {
    public:
        virtual ~IComponentPool() = default;
    };

    // Component Pool

    template<typename TComponent, size_t ENTITIES_COUNT = STANDARD_ENTITIES>
    class ComponentPool : public IComponentPool<ENTITIES_COUNT> {
    public:
        static_assert(std::is_default_constructible_v<TComponent>, "Component must contains default constructor");

        ComponentPool() {
            
        }

        void InsertComponent(entity entity, TComponent component) {
            components[entity] = component;
        }
        void RemoveComponent(entity entity, TComponent component) {
            
        }
        TComponent& GetComponent(entity entity) {
            return components[entity];
        }

    private:
        std::array<TComponent, ENTITIES_COUNT> components{};
    };
}
