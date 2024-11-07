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
            assert(entities_count_ < entities_capacity_ && "Too many active entities in world");

            entity newEntity = *available_entities_.begin();
            available_entities_.erase(newEntity);

            entities_count_++;
            return newEntity;
        }
        
        void DestroyEntity(const entity entity) {
            assert(entity < entities_capacity_ && "Entity out of world range");
            assert(entities_count_ > 0 && "All entities already destroyed");

            signatures[entity].reset();

            assert(!available_entities_.contains(entity) && "Entity already destroyed");
            available_entities_.insert(entity);
            entities_count_--;
        }
        
    public: // Component Pools

        template <typename TComponent>
        void RegisterComponent() {
            type_index component_type = TypeIndexator<TComponent>::value();
            assert(!component_pools_.contains(component_type) && "Component already registered");

            auto pool = std::make_shared<ComponentPool<TComponent>>(entities_capacity_);
            auto interfacePool = std::static_pointer_cast<AbstractComponentPool>(pool);
            component_pools_.insert_or_assign(component_type, interfacePool);

            static size_t component_counter = 0;
            component_indexes_.insert_or_assign(component_type, component_counter++);
        };

        // UnregisterComponent is all lost feature

        template <typename TComponent>
        size_t GetComponentTypeIndex() {
            type_index component_type = TypeIndexator<TComponent>::value();
            assert(component_pools_.contains(component_type) && "Component is not registered");

            return component_indexes_[component_type];
        }

    public: // Components

        template <typename TComponent>
        void AddComponent(const entity entity, TComponent component) {
            auto pool = GetComponentPool<TComponent>();
            pool->InsertComponent(entity, component);
            
            assert(entity < signatures.size() && "Entity out of range");
            dynamic_bitset& signature = signatures[entity];

            size_t index = GetComponentTypeIndex<TComponent>();
            signature.set(index, true);
        };
        
        template <typename TComponent>
        void RemoveComponent(const entity entity) {
            assert(entity < signatures.size() && "Entity out of range");
            dynamic_bitset& signature = signatures[entity];

            size_t index = GetComponentTypeIndex<TComponent>();
            signature.set(index, false);
        };
        
        template <typename TComponent>
        TComponent& GetComponent(const entity entity) {
            auto pool = GetComponentPool<TComponent>();
            return pool->GetComponent(entity);
        };
        
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

            if (component_pools_.find(component_type) == component_pools_.end()) {
                RegisterComponent<TComponent>();
                return component_pools_[component_type];
            }
            
            std::shared_ptr<AbstractComponentPool> componentPool = component_pools_[component_type];
            return std::static_pointer_cast<ComponentPool<TComponent>>(componentPool);
        }

        template <typename TComponent>
        std::shared_ptr<ComponentPool<TComponent>> GetComponentPool() {
            type_index component_type = TypeIndexator<TComponent>::value();

            assert(component_pools_.find(component_type) != component_pools_.end() && "ComponentPool can't found, register component");
            
            std::shared_ptr<AbstractComponentPool> componentPool = component_pools_[component_type];
            return std::static_pointer_cast<ComponentPool<TComponent>>(componentPool);
        }
        
    public: // Readonly Data

    public: // Data Modification
        void resize_entities(uint32_t new_size) {
            if (new_size == entities_capacity_) return;

            if (entities_capacity_ < new_size) {
                for (entity entity = entities_capacity_; entity < new_size; entity++) {
                    available_entities_.insert(entity);
                }
            }
            else {
                for (entity entity = new_size - 1; entity >= entities_capacity_; entity--) {
                    assert(available_entities_.contains(entity) && "Can't erase created entities");
                    available_entities_.erase(entity);
                }
            }

            signatures.resize(new_size);
            entities_capacity_ = new_size;
        }
        void resize_entity(uint32_t new_size) {
            if (new_size == entities_capacity_) return;

            for (auto i = 0; i < signatures.size(); i++) {
                auto& signature = signatures[i];
                signature.resize(new_size);
            }

            entity_capacity_ = new_size;
        }
        void reserve_component_pools(uint32_t new_capacity) {
            if (new_capacity == pools_capacity_) return;
            assert(component_pools_.size() <= new_capacity && "New capacity will erase registered components");

            component_pools_.reserve(new_capacity);
            component_indexes_.reserve(new_capacity);
            pools_capacity_ = new_capacity;
        }

    private: // Data
        uint32_t entities_capacity_ = 0;
        uint32_t entity_capacity_ = 0;
        uint32_t pools_capacity_ = 0;

        std::vector<dynamic_bitset> signatures;
        std::set<entity> available_entities_;
        uint32_t entities_count_ = 0;

        std::unordered_map<type_index, std::shared_ptr<AbstractComponentPool>> component_pools_;
        std::unordered_map<type_index, size_t> component_indexes_;
    };
}