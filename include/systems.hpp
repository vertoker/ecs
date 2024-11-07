#pragma once

#include "world.hpp"

#include <atomic>
#include <vector>
#include <unordered_map>
#include <memory>
#include <queue>
#include <bitset>
#include <set>

namespace ecs {

    class Systems;

    // Base System

    class System {
    public:
        virtual ~System() = default;
        World& world() { return *world_; }
        
        template <typename TComponent>
        std::vector<TComponent>::iterator begin() { return world().begin<TComponent>(); };
        template <typename TComponent>
        std::vector<TComponent>::iterator end() { return world().end<TComponent>(); };

    private:
        std::unique_ptr<World> world_;

        friend Systems;
    };

    // Default System Interfaces (you can add yours)

    class ICollectionSystem {
    public:
        virtual void execute() = 0;
    };

    class IInitSystem : public ICollectionSystem {
    public:
        void execute() override { init(); } 
        virtual void init() = 0;
    };

    class IRunSystem : public ICollectionSystem {
    public:
        void execute() override { run(); } 
        virtual void run() = 0;
    };

    class IDestroySystem : public ICollectionSystem {
    public:
        void execute() override { destroy(); } 
        virtual void destroy() = 0;
    };

    // Core Systems

    template <typename TSystem> requires std::derived_from<TSystem, ICollectionSystem>
    class SystemCollection {
    public:
        SystemCollection() = default;

        void execute() {
            for (auto it = systems.begin(); it != systems.end(); ++it) {
                auto& ptr = *it;
                ptr->execute();
            }
        }

        void AddSystem(std::shared_ptr<TSystem> system) {
            systems.insert(system);
        }
        void RemoveSystem(std::shared_ptr<TSystem> system) {
            systems.erase(system);
        }
        void Clear() {
            systems.clear();
        }

    private:
        // it uses ptr as hashable obj while system now is non const
        std::unordered_set<std::shared_ptr<ICollectionSystem>> systems{};
    };

    class Systems {
    public:
        Systems(World& world) : world_{world} { }
        ~Systems() = default;

    public:
        template<typename TSystem> requires std::derived_from<TSystem, System>
        std::shared_ptr<TSystem> RegisterSystem() {
            //static_assert(std::is_default_constructible_v<TSystem>, "System must contains default constructor");
            type_index systemType = TypeIndexator<TSystem>::value();

            assert(systems_.find(systemType) == systems_.end() && "Registering system more than once");

            auto system = std::make_shared<TSystem>();
            auto baseSystem = std::static_pointer_cast<System>(system);

            baseSystem->world_ = std::make_unique<World>(world_); // TODO refactor
            systems_.insert_or_assign(systemType, baseSystem);
            return system;
        }

        template<typename TSystem> requires std::derived_from<TSystem, System>
        void UnregisterSystem() {
            type_index systemType = TypeIndexator<TSystem>::value();

            assert(systems_.find(systemType) == systems_.end() && "Can't find system in systems");
            
            systems_.erase(systemType);
        }
        
    private:
        World world_;
        std::unordered_map<type_index, std::shared_ptr<System>> system_collections_{}; // type of interface
        std::unordered_map<type_index, std::shared_ptr<System>> systems_{}; // type of 
    };
}