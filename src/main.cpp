
#include "ecs.hpp"

#include <iostream>

struct A {};
struct B : A { int v1; };
struct C : B { int v2; C(int v2) : v2{v2} {} };

int main(int argc, char* argv[]) {
    std::cout << ecs::TypeIndexator<A>::value();
    std::cout << ecs::TypeIndexator<B>::value();
    std::cout << ecs::TypeIndexator<C>::value();
    std::cout << ecs::TypeIndexator<B>::value();
    std::cout << ecs::TypeIndexator<A>::value();
    std::cout << std::endl;

    ecs::World ecsWorld{};
    
    ecs::entity entity1 = ecsWorld.CreateEntity();
    ecs::entity entity2 = ecsWorld.CreateEntity();
    ecs::entity entity3 = ecsWorld.CreateEntity();

    ecsWorld.RegisterComponent<A>();
    ecsWorld.RegisterComponent<B>();
    ecsWorld.RegisterComponent<C>();

    ecsWorld.AddComponent(entity1, A{});
    ecsWorld.AddComponent(entity2, B{.v1=1});
    ecsWorld.AddComponent(entity2, C(2));

    return 0;
}
