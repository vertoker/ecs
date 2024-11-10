#pragma once

#include "base.hpp"
#include "types.hpp"

#include <vector>
#include <type_traits>
#include <cassert>
#include <set>

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
        class iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;

            iterator(std::set<entity>::iterator setIter, std::vector<TComponent>& vec)
                : _setIter(setIter), _vec{vec} {}
            
            TComponent& operator*() {
                auto& entity = *_setIter;
                return _vec.at(entity);
            }
            TComponent* operator->() {
                auto entity = *_setIter;
                return *_vec[entity];
            }

            iterator& operator++() { // Prefix increment
                ++_setIter;
                return *this;
            }
            iterator operator++(int) { // Postfix increment
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            [[nodiscard]] friend bool operator== (const iterator& a, const iterator& b)
                { return a._setIter == b._setIter; };
            [[nodiscard]] friend bool operator!= (const iterator& a, const iterator& b)
                { return a._setIter != b._setIter; };

        private:
            std::set<entity>::iterator _setIter;
            std::vector<TComponent>& _vec;
        };
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
                pool->_components = _components;
                return pool;
            } else {
                assert(!"Cannot clone component pool with a nin copy constructible component");
                return nullptr;
            }
        }

        void reserve(size_t new_capacity) {
            _components.reserve(new_capacity);
        }
        void resize(size_t new_size) {
            _components.resize(new_size);
        }
        void shrink_to_fit() {
            _components.shrink_to_fit();
        }

        void clear() {
            _components.clear();
            _entities.clear();
        }
        void reset() {
            clear();
            shrink_to_fit();
        }
        
        void InsertComponent(const entity entity, TComponent& component) {
            assert(entity < _components.size() && "Entity out of range");
            _components[entity] = component;
            _entities.insert(entity);
        }
        void RemoveComponent(const entity entity) {
            assert(entity < _components.size() && "Entity out of range");
            assert(_entities.contains(entity) && "Entity doesn't assign");
            _entities.erase(entity);
        }
        TComponent& GetComponent(const entity entity) {
            assert(entity < _components.size() && "Entity out of range");
            return _components[entity];
        }

        [[nodiscard]] std::vector<TComponent>::iterator begin_comp_all() { return _components.begin(); }
        [[nodiscard]] std::vector<TComponent>::iterator end_comp_all() { return _components.end(); }

        // e = entity, iterate components only for entities
        [[nodiscard]] iterator begin_comp_active() {
            auto entities = _entities.begin();
            auto& components = _components;
            return iterator{entities, components};
        }
        [[nodiscard]] iterator end_comp_active() {
            auto entities = _entities.end();
            auto& components = _components;
            return iterator{entities, components};
        }

    private:
        std::vector<TComponent> _components{};
        std::set<entity> _entities{};
    };
}
