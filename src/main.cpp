
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

class PositionSystem : public ecs::System, public ecs::IRunSystem {
public:
    void run() override {
        for (auto it = world().ebegin<Position>(); it != world().eend<Position>(); ++it) {
            auto& component = *it;
            component.x += 1;
            component.y += 1;
            component.z += 1;
        } // doesn't save iterator
    }
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
    auto runSystems = systems.RegisterSystemInterface<ecs::SystemCollection<ecs::IRunSystem>>();
    auto posSystem = systems.RegisterSystem<PositionSystem>();
    runSystems->AddSystem(posSystem);

    for (auto it = world.begin<Position>(); it != world.end<Position>(); ++it) {
        auto& component = *it;
        std::cout << component.x;
    }
    std::cout << std::endl;

    systems.ExecuteCollectionInterface<ecs::IRunSystem>();

    for (auto it = world.begin<Position>(); it != world.end<Position>(); ++it) {
        auto& component = *it;
        std::cout << component.x;
    }
    std::cout << std::endl;

    return 0;
}
