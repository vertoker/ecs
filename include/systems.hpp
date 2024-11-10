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
        
    private:
        std::unique_ptr<World> world_;

        friend Systems;
    };

    // Default System Interfaces (you can add yours)

    class ISystem {
    public:
        virtual void execute() = 0;
    };

    class IInitSystem : public ISystem {
    public:
        void execute() override { init(); } 
        virtual void init() = 0;
    };

    class IRunSystem : public ISystem {
    public:
        void execute() override { run(); } 
        virtual void run() = 0;
    };

    class IDestroySystem : public ISystem {
    public:
        void execute() override { destroy(); } 
        virtual void destroy() = 0;
    };

    template <typename TSystem> requires std::derived_from<TSystem, ISystem>
    class SystemCollection : public ISystem {
    public:
        SystemCollection() = default;
        ~SystemCollection() = default;

        void execute() override {
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
        std::unordered_set<std::shared_ptr<ISystem>> systems{};
    };


    // Core Systems

    template<class TSystem>
    concept is_system = std::derived_from<TSystem, System>;
    
    template<class TSystemInterface>
    concept is_system_interface = std::derived_from<TSystemInterface, ISystem>;

    class Systems {
    public:
        Systems(World& world) : world_{world} { }
        ~Systems() = default;

    public: // Systems

        template<class TSystem> requires is_system<TSystem>
        [[nodiscard]] std::shared_ptr<TSystem> CreateSystem() {
            //static_assert(std::is_default_constructible_v<TSystem>, "System must contains default constructor");
            type_index system_type = TypeIndexator<TSystem>::value();
            assert(!_systems.contains(system_type) && "Already exists same type system");

            auto system = std::make_shared<TSystem>();
            auto baseSystem = std::static_pointer_cast<System>(system);
            // TODO maybe later refactor
            baseSystem->world_ = std::make_unique<World>(world_);
            _systems.insert_or_assign(system_type, baseSystem);
            return system;
        }

        template<class TSystem> requires is_system<TSystem>
        void DestroySystem() {
            type_index system_type = TypeIndexator<TSystem>::value();
            assert(_systems.contains(system_type) && "Can't find system in systems");
            
            _systems.erase(system_type);
        }
        
    public: // System Interfaces

        template<class TSystemInterface> requires is_system_interface<TSystemInterface>
        [[nodiscard]] std::shared_ptr<SystemCollection<TSystemInterface>> CreateCollectionInterface() {
            return CreateInterface<SystemCollection<TSystemInterface>>();
        }
        template<class TSystemInterface> requires is_system_interface<TSystemInterface>
        [[nodiscard]] std::shared_ptr<TSystemInterface> CreateInterface() {
            type_index system_interface_type = TypeIndexator<TSystemInterface>::value();
            assert(!_system_collections.contains(system_interface_type) && "Already exists same type system interface");

            auto system_collection = std::make_shared<TSystemInterface>();
            auto system_collection_interface = std::static_pointer_cast<ISystem>(system_collection);
            _system_collections.insert_or_assign(system_interface_type, system_collection_interface);
            return system_collection;
        }

        
        template<class TSystemInterface> requires is_system_interface<TSystemInterface>
        void DestroyCollectionInterface() {
            DestroyInterface<SystemCollection<TSystemInterface>>();
        }
        template<class TSystemInterface> requires is_system_interface<TSystemInterface>
        void DestroyInterface() {
            type_index system_interface_type = TypeIndexator<TSystemInterface>::value();
            assert(_system_collections.contains(system_interface_type) && "Can't find system interface in systems interfaces");

            _system_collections.erase(system_interface_type);
        }
        
    public: // System Interfaces Execution

        template<typename TSystemInterface> requires is_system_interface<TSystemInterface>
        void ExecuteInterface() {
            type_index system_interface_type = TypeIndexator<TSystemInterface>::value();
            Execute(system_interface_type);
        }
        template<typename TSystemInterface> requires is_system_interface<TSystemInterface>
        void ExecuteCollectionInterface() {
            type_index system_interface_type = TypeIndexator<SystemCollection<TSystemInterface>>::value();
            Execute(system_interface_type);
        }

        void Execute(const type_index& system_interface_type) {
            for (auto it = _system_collections.begin(); it != _system_collections.end(); it++) {
                auto& pair = *it;
                if (pair.first != system_interface_type) continue;
                pair.second->execute(); 
            }
        }
        
    private:
        World world_;
        std::unordered_map<type_index, std::shared_ptr<System>> _systems{};
        std::unordered_map<type_index, std::shared_ptr<ISystem>> _system_collections{};
    };
}