#pragma once
#include "vulkan_object.h"
namespace libplayground {
    namespace vk {
        namespace vulkan {
            class device : public vulkan_object {
            public:
                device(std::shared_ptr<vulkan_object> instance, std::shared_ptr<vulkan_object> surface, bool validation_layers_enabled);
                VkPhysicalDevice get_physical_device();
            private:
                VkPhysicalDevice m_physical_device;
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
            class swapchain : public vulkan_object {
            public:
                swapchain(std::shared_ptr<vulkan_object> device, std::shared_ptr<vulkan_object> surface, GLFWwindow* window);
            };
        }
    }
}