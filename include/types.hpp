#pragma once

#include <stdint.h>
#include <atomic>

namespace ecs {
    using type_index_t = std::uint32_t;

    struct type_index_counter final {
        [[nodiscard]] static type_index_t next() noexcept {
            static std::atomic<type_index_t> counter{};
            return counter++;
        }
    };

    template <typename Type>
    struct type_index final {
        [[nodiscard]] static type_index_t value() noexcept {
            static const type_index_t value = ecs::type_index_counter::next();
            return value;
        }
    };
}
