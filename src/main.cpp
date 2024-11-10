
#include "ecs.hpp"

#include <iostream>

struct A {};
struct B : A { int v1; };
struct Position {
    float x, y, z;
    Position() : x{0}, y{0}, z{0} { }
    Position(float x, float y, float z) 
        : x{x}, y{y}, z{z} { }
};

class PositionSystem : public ecs::System, public ecs::IRunSystem, public ecs::IInitSystem {
public:
    void init() override {
        _pool = world().GetPool<Position>();
    }

    void run() override {
        for (auto it = _pool->begin_comp_active(); it != _pool->end_comp_active(); ++it) {
            auto& component = *it;
            component.x += 1;
            component.y += 1;
            component.z += 1;
        }
    }
private:
    std::shared_ptr<ecs::ComponentPool<Position>> _pool;
};

int main(int argc, char* argv[]) {
    std::cout << ecs::TypeIndexator<A>::value();
    std::cout << ecs::TypeIndexator<B>::value();
    std::cout << ecs::TypeIndexator<ecs::SystemCollection<ecs::IInitSystem>>::value();
    std::cout << ecs::TypeIndexator<ecs::SystemCollection<ecs::IRunSystem>>::value();
    std::cout << std::endl;

    // World
    
    ecs::World world{3, 2};
    
    ecs::entity entity1 = world.CreateEntity();
    ecs::entity entity2 = world.CreateEntity();
    ecs::entity entity3 = world.CreateEntity();

    world.RegisterComponent<A>();
    //world.RegisterComponent<B>();
    world.RegisterComponent<Position>();

    world.InsertComponent(entity1, A{});
    world.InsertComponent(entity1, A{});
    //world.AddComponent(entity1, A{});
    world.InsertComponent(entity2, Position());

    auto& signature2 = world.GetSignature(entity2);

    // Systems

    ecs::Systems systems{world};
    auto runSystems = systems.CreateCollectionInterface<ecs::IRunSystem>();
    auto initSystems = systems.CreateCollectionInterface<ecs::IInitSystem>();

    auto posSystem = systems.CreateSystem<PositionSystem>();
    initSystems->AddSystem(posSystem);
    runSystems->AddSystem(posSystem);

    auto position_pool = world.GetPool<Position>();

    // Loop

    for (auto it = position_pool->begin_comp_all(); it != position_pool->end_comp_all(); ++it) {
        auto& component = *it;
        std::cout << component.x;
    }
    std::cout << std::endl;

    systems.ExecuteCollectionInterface<ecs::IInitSystem>();

    systems.ExecuteCollectionInterface<ecs::IRunSystem>();
    systems.ExecuteCollectionInterface<ecs::IRunSystem>();
    systems.ExecuteCollectionInterface<ecs::IRunSystem>();
    systems.ExecuteCollectionInterface<ecs::IRunSystem>();
    systems.ExecuteCollectionInterface<ecs::IRunSystem>();

    for (auto it = position_pool->begin_comp_all(); it != position_pool->end_comp_all(); ++it) {
        auto& component = *it;
        std::cout << component.x;
    }
    std::cout << std::endl;

    return 0;
}
