#pragma once

#include "base.hpp"
#include "types.hpp"

#include <vector>
#include <type_traits>
#include <cassert>

namespace ecs {

    // Interface Component Pool
    
    class IComponentPool {
    public:
        virtual ~IComponentPool() = default;
    };

    // Component Pool

    template<typename TComponent>
    class ComponentPool : public IComponentPool {
        static_assert(std::is_move_constructible_v<TComponent>, "Cannot create pool for component which is not move constructible");
	    static_assert(std::is_destructible_v<TComponent>, "Cannot create pool for component which is not destructible");
        static_assert(std::is_default_constructible_v<TComponent>, "Cannot create pool for component which doesn't has default constructor");

    public:
        // Constructors
        ComponentPool() = default;
        ~ComponentPool() = default;
        // Move
	    ComponentPool(ComponentPool&&)				 = default;
	    ComponentPool& operator=(ComponentPool&&)		 = default;
        // Copy
	    ComponentPool(const ComponentPool&)			 = default;
	    ComponentPool& operator=(const ComponentPool&) = default;

        ComponentPool(uint32_t reserve_entities = DEFAULT_ENTITIES_CAPACITY) {
            resize(reserve_entities);
        }

        std::shared_ptr<IComponentPool> clone() const {
            if constexpr (std::is_copy_constructible_v<TComponent>) {
                auto pool = std::make_shared<ComponentPool<TComponent>>();
                pool->components = components;
                return pool;
            } else {
                assert(!"Cannot clone component pool with a nin copy constructible component");
                return nullptr;
            }
        }

        void reserve(std::size_t new_capacity) { components.reserve(new_capacity); }
        void resize(size_t new_size) { components.resize(new_size); }
        void shrink_to_fit() { components.shrink_to_fit(); }

        void clear() { components.clear(); }
        void reset() { clear(); shrink_to_fit(); }

        void InsertComponent(const entity entity, TComponent& component) {
            assert(entity < components.size() && "Can't insert component for entity");
            components[entity] = component;
        }
        TComponent& GetComponent(const entity entity) {
            return components[entity];
        }

        constexpr std::vector<TComponent>::iterator begin() { return components.begin(); } // Iterate with entity filter
        constexpr std::vector<TComponent>::iterator end() { return components.end(); }

    private:
        std::vector<TComponent> components;
    };
}
