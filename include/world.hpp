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
        
    public: // Entities

        entity CreateEntity() {
            assert(_available_entities.size() > 0 && "Doesn't have available entities, expand entities capacity");

            entity newEntity = *_available_entities.begin();
            _available_entities.erase(newEntity);

            _entities_count++;
            return newEntity;
        }
        
        void DestroyEntity(const entity entity) {
            assert(entity < _entities_capacity && "Entity out of world range");
            assert(_entities_count > 0 && "All entities already destroyed");

            _signatures[entity].reset();

            assert(!_available_entities.contains(entity) && "Entity already destroyed");
            _available_entities.insert(entity);
            _entities_count--;
        }

        bool ExistsEntity(const entity entity) {
            assert(entity < _entities_capacity && "Entity out of range");
            if (_entities_count == 0) return false;
            return !_available_entities.contains(entity);
        }

        dynamic_bitset& GetSignature(const entity entity) {
            assert(entity < _entities_capacity && "Entity out of range");
            assert(!_available_entities.contains(entity) && "Entity doesn't created");
            return _signatures[entity];
        }
        void SetSignature(const entity entity, dynamic_bitset signature) {
            assert(entity < _entities_capacity && "Entity out of range");
            assert(!_available_entities.contains(entity) && "Entity doesn't created");
            _signatures[entity] = signature;
        }

        std::vector<dynamic_bitset>::iterator begin(const entity entity) { // TODO fix filtering
            assert(entity < _entities_capacity && "Entity out of range");
            assert(!_available_entities.contains(entity) && "Entity doesn't created");
            return _signatures.begin();
        };
        std::vector<dynamic_bitset>::iterator end(const entity entity) {
            assert(entity < _entities_capacity && "Entity out of range");
            assert(!_available_entities.contains(entity) && "Entity doesn't created");
            return _signatures.end();
        };
        
    public: // Component Pools

        template <typename TComponent>
        void RegisterComponent() {
            type_index component_type = TypeIndexator<TComponent>::value();
            assert(!_component_pools.contains(component_type) && "Component already registered");

            auto pool = std::make_shared<ComponentPool<TComponent>>(_entities_capacity);
            auto interfacePool = std::static_pointer_cast<IComponentPool>(pool);
            _component_pools.insert_or_assign(component_type, interfacePool);

            static size_t component_counter = 0;
            _component_indexes.insert_or_assign(component_type, component_counter++);
        };

        // UnregisterComponent is all lost feature

        template <typename TComponent>
        size_t GetComponentTypeIndex() {
            type_index component_type = TypeIndexator<TComponent>::value();
            assert(_component_pools.contains(component_type) && "Component is not registered");

            return _component_indexes[component_type];
        }

    public: // Components
    
        template <typename TComponent>
        void AddComponent(const entity entity, TComponent component) {
            assert(!ContainsComponent<TComponent>(entity) && "Already contains this component in this entity");

            auto pool = GetComponentPool<TComponent>();
            pool->InsertComponent(entity, component);
            
            assert(entity < _signatures.size() && "Entity out of range");
            dynamic_bitset& signature = _signatures[entity];
            size_t index = GetComponentTypeIndex<TComponent>();
            signature.set(index, true);
        }

        template <typename TComponent>
        void InsertComponent(const entity entity, TComponent component) {
            auto pool = GetComponentPool<TComponent>();
            pool->InsertComponent(entity, component);
            
            assert(entity < _signatures.size() && "Entity out of range");
            dynamic_bitset& signature = _signatures[entity];
            size_t index = GetComponentTypeIndex<TComponent>();
            signature.set(index, true);
        };
        
        template <typename TComponent>
        void RemoveComponent(const entity entity) {
            assert(entity < _entities_capacity && "Entity out of range");
            dynamic_bitset& signature = _signatures[entity];
            size_t index = GetComponentTypeIndex<TComponent>();
            signature.set(index, false);
        };
        
        template <typename TComponent>
        TComponent& GetComponent(const entity entity) {
            auto pool = GetComponentPool<TComponent>();
            return pool->GetComponent(entity);
        };

        template <typename TComponent>
        bool ContainsComponent(const entity entity) {
            assert(entity < _entities_capacity && "Entity out of range");
            dynamic_bitset& signature = _signatures[entity];
            size_t index = GetComponentTypeIndex<TComponent>();
            return signature.get(index);
        }
        
        template <typename TComponent>
        std::vector<TComponent>::iterator begin() {
            auto pool = GetComponentPool<TComponent>();
            return pool->begin();
        };
        template <typename TComponent>
        std::vector<TComponent>::iterator end() {
            auto pool = GetComponentPool<TComponent>();
            return pool->end();
        };

    private:
        template <typename TComponent>
        std::shared_ptr<ComponentPool<TComponent>> GetOrCreateComponentPool() {
            type_index component_type = TypeIndexator<TComponent>::value();

            if (_component_pools.find(component_type) == _component_pools.end()) {
                RegisterComponent<TComponent>();
                return _component_pools[component_type];
            }
            
            std::shared_ptr<IComponentPool> componentPool = _component_pools[component_type];
            return std::static_pointer_cast<ComponentPool<TComponent>>(componentPool);
        }

        template <typename TComponent>
        std::shared_ptr<ComponentPool<TComponent>> GetComponentPool() {
            type_index component_type = TypeIndexator<TComponent>::value();

            assert(_component_pools.find(component_type) != _component_pools.end() && "ComponentPool can't found, register component");
            
            std::shared_ptr<IComponentPool> componentPool = _component_pools[component_type];
            return std::static_pointer_cast<ComponentPool<TComponent>>(componentPool);
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

            _signatures.resize(new_size);
            _entities_capacity = new_size;
        }
        void resize_entity(uint32_t new_size) {
            if (new_size == _entities_capacity) return;

            for (auto i = 0; i < _signatures.size(); i++) {
                auto& signature = _signatures[i];
                signature.resize(new_size);
            }

            _entity_capacity = new_size;
        }
        void reserve_component_pools(uint32_t new_capacity) {
            if (new_capacity == _pools_capacity) return;
            assert(_component_pools.size() <= new_capacity && "New capacity will erase registered components");

            _component_pools.reserve(new_capacity);
            _component_indexes.reserve(new_capacity);
            _pools_capacity = new_capacity;
        }

    private: // Data
        uint32_t _entities_capacity = 0;
        uint32_t _entity_capacity = 0;
        uint32_t _pools_capacity = 0;

        std::vector<dynamic_bitset> _signatures; // make map
        std::set<entity> _available_entities;
        uint32_t _entities_count = 0;

        std::unordered_map<type_index, std::shared_ptr<IComponentPool>> _component_pools;
        std::unordered_map<type_index, size_t> _component_indexes;
    };
}