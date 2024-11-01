
#include "ecs.hpp"

#include <iostream>

struct A {};
struct B : A {};
struct C : B {};

int main(int argc, char* argv[]) {
    std::cout << ecs::type_index<A>::value() << std::endl;
    std::cout << ecs::type_index<B>::value() << std::endl;
    std::cout << ecs::type_index<C>::value() << std::endl;
    std::cout << ecs::type_index<B>::value() << std::endl;
    std::cout << ecs::type_index<A>::value() << std::endl;
    std::cin.get();

    return 0;
}
