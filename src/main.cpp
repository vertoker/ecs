
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

class PositionSystem : public ecs::System {
    void run() {
        
    }
};

int main(int argc, char* argv[]) {
    std::cout << ecs::TypeIndexator<A>::value();
    std::cout << ecs::TypeIndexator<B>::value();
    //std::cout << ecs::TypeIndexator<C>::value();
    std::cout << ecs::TypeIndexator<B>::value();
    std::cout << ecs::TypeIndexator<A>::value();
    std::cout << std::endl;

    ecs::World ecsWorld{3, 2, 4};
    ecs::Systems ecsSystems{ecsWorld};
    
    ecs::entity entity1 = ecsWorld.CreateEntity();
    ecs::entity entity2 = ecsWorld.CreateEntity();
    ecs::entity entity3 = ecsWorld.CreateEntity();

    ecsWorld.RegisterComponent<A>();
    //ecsWorld.RegisterComponent<B>();
    ecsWorld.RegisterComponent<Position>();

    ecsWorld.AddComponent(entity1, A{});
    //ecsWorld.AddComponent(entity2, B{.v1=1});
    ecsWorld.AddComponent(entity2, Position());

    ecsSystems.RegisterSystem<PositionSystem>();

    return 0;
}
