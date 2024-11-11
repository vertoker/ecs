#pragma once

#include "dynamic_bitset.hpp"

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

    class World {
    public:
        World(uint32_t default_entities_capacity = DEFAULT_ENTITIES_CAPACITY, 
            uint32_t default_entity_capacity = DEFAULT_ENTITY_CAPACITY) {
            
            resize_entities(default_entities_capacity);
            resize_entity(default_entity_capacity);
            reserve_component_pools(default_entity_capacity);
        }

    private:
        #define assert_entity_range(entity) assert(entity < _entities_capacity && "Entity out of range");
        #define assert_component_types() assert(_components.size() <= _entity_capacity && "Count of registered components is bigger than entity capacity");
        #define assert_signature_exists(entity) assert(_signatures.contains(entity) && "Entity's signature doesn't exists");
        #define assert_destroyed_entity(entity) assert(_available_entities.contains(entity) && "Entity doesn't destroyed");
        #define assert_created_entity(entity) assert(!_available_entities.contains(entity) && "Entity doesn't created");

    public:
        class iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;

            constexpr iterator(uint8_t* ptr, const uint8_t& offset)
                : _ptr(ptr), _offset{offset} {}

            bool operator*() const
            {
    	        return true;
            }

            iterator& operator++() { // Prefix increment
                if (_offset >= 7) {
                    ++_ptr;
                    _offset = 0;
                }
                else {
                    ++_offset;
                }
                return *this;
            }
            iterator operator++(int) { // Postfix increment
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            [[nodiscard]] friend bool operator== (const iterator& a, const iterator& b)
                { return a._ptr == b._ptr && a._offset == b._offset; };
            [[nodiscard]] friend bool operator!= (const iterator& a, const iterator& b)
                { return a._ptr != b._ptr || a._offset != b._offset; };

        private:
            uint8_t* _ptr;
            uint8_t _offset;
        };
        
    public: // Entities

        [[nodiscard]] entity CreateEntity() {
            assert(_available_entities.size() > 0 && "Doesn't have available entities, do expand entities capacity");

            entity newEntity = *_available_entities.begin();

            assert_destroyed_entity(newEntity);
            _available_entities.erase(newEntity);
            _signatures.insert_or_assign(newEntity, dynamic_bitset{_entity_capacity});

            _entities_count++;
            return newEntity;
        }
        
        void DestroyEntity(const entity entity) {
            assert(_entities_count > 0 && "All entities already destroyed");
            
            RemoveAllComponents(entity);
            
            assert_entity_range(entity);
            assert_signature_exists(entity);
            _signatures.erase(entity);

            assert_created_entity(entity);
            _available_entities.insert(entity);
            
            _entities_count--;
        }

        bool ExistsEntity(const entity entity) {
            if (entity >= _entities_capacity) return false;
            if (_entities_count == 0) return false;
            return !_available_entities.contains(entity);
        }

        [[nodiscard]] dynamic_bitset& GetSignature(const entity entity) {
            assert_entity_range(entity);
            assert_signature_exists(entity);
            assert_created_entity(entity);

            return _signatures[entity];
        }
        void SetSignature(const entity entity, dynamic_bitset signature) {
            assert_entity_range(entity);
            assert_signature_exists(entity);
            assert_created_entity(entity);

            _signatures[entity] = signature;
        }
        
    public: // Component Pools

        template <typename TComponent>
        void RegisterComponent() {
            type_index component_type = TypeIndexator<TComponent>::value();
            assert(!_component_pools.contains(component_type) && "Component already registered");

            auto pool = std::make_shared<ComponentPool<TComponent>>(_entities_capacity);
            auto interfacePool = std::static_pointer_cast<IComponentPool>(pool);
            _component_pools.insert_or_assign(component_type, interfacePool);

            _component_indexes.insert_or_assign(component_type, _components.size());
            _components.emplace_back(component_type);
        }

        // UnregisterComponent is a lost feature, to hard to implement

        template <typename TComponent>
        [[nodiscard]] size_t GetComponentTypeIndex() {
            type_index component_type = TypeIndexator<TComponent>::value();
            return GetComponentTypeIndex(component_type);
        }
        [[nodiscard]] size_t GetComponentTypeIndex(const type_index component_type) {
            assert(_component_pools.contains(component_type) && "Component is not registered");
            return _component_indexes[component_type];
        }

    public: // Components
    
        template <typename TComponent>
        void AddComponent(const entity entity, TComponent component) {
            assert(!ContainsComponent<TComponent>(entity) && "Already contains this component in this entity");
            InsertComponent(entity, component);
        }

        template <typename TComponent>
        void InsertComponent(const entity entity, TComponent component) {
            assert_entity_range(entity);

            auto pool = GetPool<TComponent>();
            pool->InsertComponent(entity, component);
            
            assert_signature_exists(entity);
            assert_created_entity(entity);

            dynamic_bitset& signature = _signatures[entity];
            size_t index = GetComponentTypeIndex<TComponent>();
            signature.set(index, true);
        }
        
        template <typename TComponent>
        void RemoveComponent(const entity entity) {
            assert_entity_range(entity);
            assert_signature_exists(entity);
            assert_created_entity(entity);

            auto pool = GetPool<TComponent>();
            pool->RemoveComponent(entity);

            dynamic_bitset& signature = _signatures[entity];
            size_t index = GetComponentTypeIndex<TComponent>();
            signature.set(index, false);
        }

        void RemoveAllComponents(const entity entity) {
            auto signature = GetSignature(entity);
            assert_component_types();
            for (auto i = 0; i < _components.size(); i++)
            {
                if (!signature[i]) continue;
                auto component_type = _components[i];
                auto pool = GetPool(component_type);
                pool->RemoveComponent(entity);
            }
        }
        
        template <typename TComponent>
        [[nodiscard]] TComponent& GetComponent(const entity entity) {
            auto pool = GetPool<TComponent>();
            return pool->GetComponent(entity);
        }

        template <typename TComponent>
        [[nodiscard]] bool ContainsComponent(const entity entity) {
            assert_entity_range(entity);
            assert_signature_exists(entity);
            assert_created_entity(entity);

            dynamic_bitset& signature = _signatures[entity];
            size_t index = GetComponentTypeIndex<TComponent>();
            return signature.get(index);
        }

    public: // Iterators
    
        std::unordered_map<entity, dynamic_bitset>::iterator begin_ent_active() {
            return _signatures.begin();
        }
        std::unordered_map<entity, dynamic_bitset>::iterator end_ent_active() {
            return _signatures.end();
        }

    public: // Pools
        template <typename TComponent>
        [[nodiscard]] std::shared_ptr<ComponentPool<TComponent>> GetOrCreatePool() {
            type_index component_type = TypeIndexator<TComponent>::value();

            if (_component_pools.find(component_type) == _component_pools.end()) {
                RegisterComponent<TComponent>();
                return _component_pools[component_type];
            }
            
            std::shared_ptr<IComponentPool> componentPool = _component_pools[component_type];
            return std::static_pointer_cast<ComponentPool<TComponent>>(componentPool);
        }

        template <typename TComponent>
        [[nodiscard]] std::shared_ptr<ComponentPool<TComponent>> GetPool() {
            type_index component_type = TypeIndexator<TComponent>::value();
            std::shared_ptr<IComponentPool> componentPool = GetPool(component_type);
            return std::static_pointer_cast<ComponentPool<TComponent>>(componentPool);
        }
        [[nodiscard]] std::shared_ptr<IComponentPool> GetPool(const type_index& component_type) {
            assert(_component_pools.find(component_type) != _component_pools.end() && "ComponentPool can't found, register component");
            
            std::shared_ptr<IComponentPool> componentPool = _component_pools[component_type];
            return componentPool;
        }

    public: // Data Modification
        void resize_entities(uint32_t new_size) {
            if (new_size == _entities_capacity) return;

            if (_entities_capacity < new_size) {
                for (entity entity = _entities_capacity; entity < new_size; entity++) {
                    _available_entities.insert(entity);
                }
            }
            else {
                for (entity entity = new_size - 1; entity >= _entities_capacity; entity--) {
                    assert(_available_entities.contains(entity) && "Can't erase created entities");
                    _available_entities.erase(entity);
                }
            }
            
            _entities_capacity = new_size;
        }
        void resize_entity(uint32_t new_size) {
            if (new_size == _entities_capacity) return;

            for (auto it = _signatures.begin(); it != _signatures.end(); it++) {
                auto& pair = *it;
                pair.second.resize(new_size);
            }

            _entity_capacity = new_size;
        }
        void reserve_component_pools(uint32_t new_capacity) {
            if (new_capacity == _pools_capacity) return;
            assert(_component_pools.size() <= new_capacity && "New capacity will erase registered components");

            _component_pools.reserve(new_capacity);
            _component_indexes.reserve(new_capacity);
            _components.reserve(new_capacity);
            _pools_capacity = new_capacity;
        }

    private: // Data
        uint32_t _entities_capacity = 0;
        uint32_t _entity_capacity = 0;
        uint32_t _pools_capacity = 0;

        std::unordered_map<entity, dynamic_bitset> _signatures;
        std::set<entity> _available_entities;
        uint32_t _entities_count = 0;

        std::unordered_map<type_index, std::shared_ptr<IComponentPool>> _component_pools;
        std::unordered_map<type_index, size_t> _component_indexes;
        std::vector<type_index> _components;
    };
}