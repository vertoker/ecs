#pragma once

#include "types.hpp"
#include "utils.hpp"

#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <memory>

namespace ecs {

    using entity = std::uint32_t;
    using component_index = std::uint32_t;

    static const uint32_t DEFAULT_ENTITIES_CAPACITY = 5000;
    static const uint32_t DEFAULT_ENTITY_CAPACITY = 32;
}
