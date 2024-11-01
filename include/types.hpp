#pragma once

#include <stdint.h>
#include <atomic>

namespace ecs {
    using type_index = std::uint32_t;

    struct TypeIndexatorCounter final {
        [[nodiscard]] static type_index next() noexcept {
            static std::atomic<type_index> counter{};
            return counter++;
        }
    };

    template <typename Type>
    struct TypeIndexator final {
        [[nodiscard]] static type_index value() noexcept {
            static const type_index value = ecs::TypeIndexatorCounter::next();
            return value;
        }
    };
}
