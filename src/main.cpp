#include "pch.h"
#include "vkplayground_app.h"
int main() {
    std::shared_ptr<vkplayground::application> app = std::make_shared<vkplayground_app>();
    try {
        app->run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}