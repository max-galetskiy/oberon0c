#include "panic.h"

void panic(const std::string& message) {
    std::cerr << "Panic: " << message << std::endl;
    std::terminate();
}