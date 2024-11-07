
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
        for (auto it = begin<Position>(); it != end<Position>(); ++it) {
            auto& component = *it;
            component.x += 1;
            component.y += 1;
            component.z += 1;
        }
    }
};

int main(int argc, char* argv[]) {
    std::cout << ecs::TypeIndexator<A>::value();
    std::cout << ecs::TypeIndexator<B>::value();
    //std::cout << ecs::TypeIndexator<C>::value();
    std::cout << ecs::TypeIndexator<B>::value();
    std::cout << ecs::TypeIndexator<A>::value();
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
    //world.AddComponent(entity1, A{});
    //world.InsertComponent(entity2, B{.v1=1});
    world.InsertComponent(entity2, Position());

    auto& signature2 = world.GetSignature(entity2);
    
    // Systems

    ecs::Systems ecsSystems{world};
    ecs::SystemCollection<ecs::IRunSystem> runSystems{};

    auto posSystem = ecsSystems.RegisterSystem<PositionSystem>();
    runSystems.AddSystem(posSystem);

    for (auto it = world.begin<Position>(); it != world.end<Position>(); ++it) {
        auto& component = *it;
        std::cout << component.x;
    }
    std::cout << std::endl;

    runSystems.execute();

    for (auto it = world.begin<Position>(); it != world.end<Position>(); ++it) {
        auto& component = *it;
        std::cout << component.x;
    }
    std::cout << std::endl;

    return 0;
}
