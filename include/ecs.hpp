#pragma once

#include "entity.hpp"
#include "component.hpp"
#include "systems.hpp"

#include "test.hpp"

#include <memory>

namespace ecs {
    class EcsWorld {
    public:
        EcsWorld() {
            entityManager = std::make_unique<EntityManager>();
            componentManager = std::make_unique<ComponentManager>();
            systemManager = std::make_unique<SystemManager>();
        }

    private:
        std::unique_ptr<EntityManager> entityManager;
        std::unique_ptr<ComponentManager> componentManager;
        std::unique_ptr<SystemManager> systemManager;
    };
}