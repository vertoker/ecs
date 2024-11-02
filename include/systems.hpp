#pragma once

#include "components.hpp"
#include "entities.hpp"

#include <set>

namespace ecs {
    class System {
    public:
        std::set<entity> entities;
    };

    class SystemManager {

    };
}