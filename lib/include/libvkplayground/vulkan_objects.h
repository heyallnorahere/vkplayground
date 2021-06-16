#pragma once
#include "vulkan_object.h"
namespace libplayground {
    namespace vk {
        namespace vulkan {
            class device : public vulkan_object {
            public:
                device(std::shared_ptr<vulkan_object> instance, std::shared_ptr<vulkan_object> surface, bool validation_layers_enabled);
            };
            class instance : public vulkan_object {
            public:
                instance(const std::string& title, bool validation_layers_enabled);
            };
            class debug_messenger : public vulkan_object {
            public:
                debug_messenger(bool validation_layers_enabled, std::shared_ptr<vulkan_object> instance);
            };
            class surface : public vulkan_object {
            public:
                surface(GLFWwindow* window, std::shared_ptr<vulkan_object> instance);
            };
        }
    }
}