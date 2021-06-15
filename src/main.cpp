#include "pch.h"
#include "window.h"
int main() {
    auto window = std::make_shared<vkplayground::window>("Vulkan playground", 800, 600);
    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::cout << extension_count << " extensions supported" << std::endl;
    while (!window->should_close()) {
        window->swap_buffers();
        window->poll_events();
    }
    return 0;
}