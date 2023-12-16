#include "Demo.hpp"

int main() {
    auto demo = std::make_unique<Demo>();
    demo->run();
    return 0;
}