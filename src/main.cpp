#include "pch.h"
#include "vkplayground.h"
int main() {
    std::shared_ptr<libplayground::vk::application> app = std::make_shared<vkplayground>();
    try {
        app->run();
    } catch (const std::exception& e) {
        spdlog::error(e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}