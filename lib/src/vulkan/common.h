#pragma once
namespace libplayground {
    namespace vk {
        namespace vulkan {
            struct swapchain_support_details {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> formats;
                std::vector<VkPresentModeKHR> present_modes;
            };
            struct queue_family_indices {
                std::optional<uint32_t> graphics_family, present_family;
                bool is_complete() {
                    return this->graphics_family.has_value() && this->present_family.has_value();
                }
            };
            swapchain_support_details query_swapchain_support(VkPhysicalDevice device, std::shared_ptr<vulkan_object> surface);
            queue_family_indices find_queue_families(VkPhysicalDevice device, std::shared_ptr<vulkan_object> surface);
        }
    }
}