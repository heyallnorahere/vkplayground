#pragma once
namespace libplayground {
    namespace vk {
        namespace debug {
            extern std::vector<const char*> validation_layers;
            void populate_VkDebugUtilsMessengerCreateInfoEXT(VkDebugUtilsMessengerCreateInfoEXT& create_info);
        }
    }
}