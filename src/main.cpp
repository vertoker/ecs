
#include "ecs.hpp"

#include <iostream>

struct A {};
struct B : A {};
struct C : B {};

int main(int argc, char* argv[]) {
    std::cout << ecs::TypeIndexator<A>::value() << std::endl;
    std::cout << ecs::TypeIndexator<B>::value() << std::endl;
    std::cout << ecs::TypeIndexator<C>::value() << std::endl;
    std::cout << ecs::TypeIndexator<B>::value() << std::endl;
    std::cout << ecs::TypeIndexator<A>::value() << std::endl;
    std::cin.get();

    return 0;
}
