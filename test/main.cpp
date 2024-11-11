
#include "ecs.hpp"

#include <iostream>

#define EXPECT_EQ(item1, item2) assert(item1 == item2 && "Items is not equals");

struct A {};
struct B : A { int v1; };
struct Position {
    float x, y, z;
    Position() : x{0}, y{0}, z{0} { }
    Position(float x, float y, float z) 
        : x{x}, y{y}, z{z} { }
};

class PositionSystem : public ecs::BaseSystem<Position> {
public:
    void run() override {
        for (auto it = _pool->begin_comp_active(); it != _pool->end_comp_active(); ++it) {
            auto& component = *it;
            component.x += 1;
            component.y += 1;
            component.z += 1;
        }
    }
};


int main(int argc, char* argv[]) {

    auto a_type = ecs::TypeIndexator<A>::value();
    EXPECT_EQ(0, a_type);
    EXPECT_EQ(0, ecs::TypeIndexator<A>::value());

    auto b_type = ecs::TypeIndexator<B>::value();
    EXPECT_EQ(1, b_type);
    EXPECT_EQ(1, ecs::TypeIndexator<B>::value());

    // World
    
    ecs::World world{3, 2};
    
    ecs::entity entity1 = world.CreateEntity();
    ecs::entity entity2 = world.CreateEntity();
    ecs::entity entity3 = world.CreateEntity();
    EXPECT_EQ(0, entity1);
    EXPECT_EQ(2, entity3);
    EXPECT_EQ(true, world.ExistsEntity(entity1));
    EXPECT_EQ(false, world.ExistsEntity(3));

    world.RegisterComponent<A>();
    //world.RegisterComponent<B>();
    world.RegisterComponent<Position>();

    world.InsertComponent(entity1, A{});
    world.InsertComponent(entity1, A{});
    //world.AddComponent(entity1, A{});
    world.InsertComponent(entity2, Position());
    
    auto& signature1 = world.GetSignature(entity1);
    auto& signature2 = world.GetSignature(entity2);

    EXPECT_EQ(true, signature1.get(0));
    EXPECT_EQ(false, signature1.get(1));
    EXPECT_EQ(false, signature2.get(0));
    EXPECT_EQ(true, signature2.get(1));

    // Systems

    ecs::Systems systems{world};
    auto initSystems = systems.CreateCollectionInterface<ecs::IInitSystem>();
    auto runSystems = systems.CreateCollectionInterface<ecs::IRunSystem>();
    auto destroySystems = systems.CreateCollectionInterface<ecs::IDestroySystem>();

    auto posSystem = systems.CreateSystem<PositionSystem>();
    initSystems->AddSystem(posSystem);
    runSystems->AddSystem(posSystem);
    destroySystems->AddSystem(posSystem);

    auto position_pool = world.GetPool<Position>();

    // Loop

    systems.ExecuteCollectionInterface<ecs::IInitSystem>();

    EXPECT_EQ(0, position_pool->GetComponent(entity1).x);
    EXPECT_EQ(0, position_pool->GetComponent(entity2).x);
    EXPECT_EQ(0, position_pool->GetComponent(entity3).x);
    for (auto it = position_pool->begin_comp_all(); it != position_pool->end_comp_all(); ++it) {
        auto& component = *it;
        std::cout << component.x;
    }
    std::cout << std::endl;

    systems.ExecuteCollectionInterface<ecs::IRunSystem>();
    runSystems->execute();
    runSystems->execute();
    runSystems->execute();
    runSystems->execute();

    EXPECT_EQ(0, position_pool->GetComponent(entity1).x);
    EXPECT_EQ(5, position_pool->GetComponent(entity2).x);
    EXPECT_EQ(0, position_pool->GetComponent(entity3).x);
    for (auto it = position_pool->begin_comp_all(); it != position_pool->end_comp_all(); ++it) {
        auto& component = *it;
        std::cout << component.x;
    }
    std::cout << std::endl;

    destroySystems->execute();

    // Destroy

    initSystems->Clear();
    runSystems->Clear();
    destroySystems->Clear();

    systems.DestroySystem<PositionSystem>();
    systems.DestroyCollectionInterface<ecs::IInitSystem>();
    systems.DestroyCollectionInterface<ecs::IRunSystem>();
    systems.DestroyCollectionInterface<ecs::IDestroySystem>();

    world.DestroyEntity(entity1);
    world.DestroyEntity(entity2);
    world.DestroyEntity(entity3);

    return 0;
}