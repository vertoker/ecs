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
    
    class System {
    protected:
        std::set<entity> entities;
    };

    class Systems {
    public:
        Systems(World& world) : world{world} {
            
        }

    public:
        template<typename TSystem>
        std::shared_ptr<TSystem> RegisterSystem() {
            static_assert(std::is_default_constructible_v<TSystem>, "System must contains default constructor");
            type_index systemType = TypeIndexator<TSystem>::value();

            assert(systems.find(systemType) != systems.end() && "Registering system more than once");

            auto system = std::make_shared<TSystem>();
            systems.insert_or_assign(systemType, system);
            return system;
        }
        template<typename TSystem>
        void RegisterSystem(std::shared_ptr<TSystem> system) {
            type_index systemType = TypeIndexator<TSystem>::value();

            assert(systems.find(systemType) != systems.end() && "Registering system more than once");

            systems.insert_or_assign(systemType, system);
        }

        template<typename TSystem>
        void UnregisterSystem() {
            static_assert(std::is_default_constructible_v<TSystem>, "System must contains default constructor");
            type_index systemType = TypeIndexator<TSystem>::value();

            assert(systems.find(systemType) == systems.end() && "Can't find system in systems");
            
            systems.erase(systemType);
        }
        
    private:
        World world;
        std::unordered_map<type_index, std::shared_ptr<System>> systems{};
    };
}