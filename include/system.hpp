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

    class Systems {
    public:
        Systems(World& world) : world_{world} { }
        ~Systems() = default;

    public:
        template<typename TSystem>
        std::shared_ptr<TSystem> RegisterSystem() {
            //static_assert(std::is_default_constructible_v<TSystem>, "System must contains default constructor");
            type_index systemType = TypeIndexator<TSystem>::value();

            assert(systems_.find(systemType) == systems_.end() && "Registering system more than once");

            auto system = std::make_shared<TSystem>();
            auto baseSystem = std::static_pointer_cast<System>(system);

            baseSystem->world_ = std::make_unique<World>(world_);
            systems_.insert_or_assign(systemType, baseSystem);
            return system;
        }
        template<typename TSystem>
        void UnregisterSystem() {
            static_assert(std::is_default_constructible_v<TSystem>, "System must contains default constructor");
            type_index systemType = TypeIndexator<TSystem>::value();

            assert(systems_.find(systemType) == systems_.end() && "Can't find system in systems");
            
            systems_.erase(systemType);
        }
        
    private:
        World world_;
        std::unordered_map<type_index, std::shared_ptr<System>> systems_{};
    };
}