#include "panic.h"

void panic(const std::string& message) {
    std::cerr << message << std::endl;
    std::terminate();
}