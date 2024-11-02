#pragma once

// std
#include <functional>

template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
	const unsigned int magicNumber = 0x9e3779b9;
	seed ^= std::hash<T>{}(v) + magicNumber + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
}
