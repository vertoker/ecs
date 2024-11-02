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

    struct component_address {
        type_index componentType;
        component_index poolIndex;

        component_address(type_index componentType, component_index poolIndex) 
            : componentType{componentType}, poolIndex{poolIndex} { }
        
        bool operator==(const component_address& other) const {
            return componentType == other.componentType && poolIndex == other.poolIndex;
        }
    };

    static const size_t RESERVED_ENTITIES = 5000;
    static const size_t RESERVED_ENTITY_COMPONENTS = 32;

    static const size_t RESERVED_COMPONENT_POOL = 32;
    static const size_t RESERVED_POOLS = 4;
}


template <>
struct std::hash<ecs::component_address>
{
    std::size_t operator()(const ecs::component_address& obj) const {
        size_t hash = 0;
        hashCombine(hash, obj.componentType, obj.poolIndex);
        return hash;
    }
};
