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

    static const size_t STANDARD_ENTITIES = 5000;
    static const size_t STANDARD_COMPONENTS = 32;

    static const size_t RESERVED_POOLS = 4;
}
