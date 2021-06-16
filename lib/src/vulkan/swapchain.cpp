#include "libvkplayground_pch.h"
#include "libvkplayground/vulkan_objects.h"
#include "libvkplayground/debug.h"
#include "common.h"
namespace libplayground {
    namespace vk {
        namespace vulkan {
            struct swapchain_create_arg {
                std::shared_ptr<vulkan_object> device, surface;
                GLFWwindow* window;
            };
            struct swapchain_destroy_arg {
                std::shared_ptr<vulkan_object> device;
            };
            static VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) {
                for (const auto& available_format : available_formats) {
                    if (available_format.format == VK_FORMAT_R8G8B8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                        return available_format;
                    }
                }
                return available_formats[0];
            }
            static VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes) {
                for (const auto& available_present_mode : available_present_modes) {
                    if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                        return available_present_mode;
                    }
                }
                return VK_PRESENT_MODE_FIFO_KHR;
            }
            static VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
                if (capabilities.currentExtent.width != UINT32_MAX) {
                    return capabilities.currentExtent;
                } else {
                    int32_t width, height;
                    glfwGetFramebufferSize(window, &width, &height);
                    return {
                        std::clamp((uint32_t)width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                        std::clamp((uint32_t)height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
                    };
                }
            }
            static void* create_swapchain(void* user_arg) {
                auto arg = (swapchain_create_arg*)user_arg;
                auto device_ = arg->device;
                auto surface_ = arg->surface;
                auto window = arg->window;
                delete arg; // we have our own copies now; we can free this
                auto physical_device = ((device*)device_.get())->get_physical_device();
                auto surface = surface_->get<VkSurfaceKHR>();
                swapchain_support_details swapchain_support = query_swapchain_support(physical_device, surface_);
                VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swapchain_support.formats);
                VkPresentModeKHR present_mode = choose_swap_present_mode(swapchain_support.present_modes);
                VkExtent2D extent = choose_swap_extent(swapchain_support.capabilities, window);
                uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;
                if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount) {
                    image_count = swapchain_support.capabilities.maxImageCount;
                }
                VkSwapchainCreateInfoKHR create_info{ };
                create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                create_info.surface = surface;
                create_info.minImageCount = image_count;
                create_info.imageFormat = surface_format.format;
                create_info.imageColorSpace = surface_format.colorSpace;
                create_info.imageExtent = extent;
                create_info.imageArrayLayers = 1;
                create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                queue_family_indices indices = find_queue_families(physical_device, surface_);
                std::array<uint32_t, 2> queue_family_indices = {
                    indices.graphics_family.value(),
                    indices.present_family.value()
                };
                if (indices.graphics_family != indices.present_family) {
                    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                    create_info.queueFamilyIndexCount = (uint32_t)queue_family_indices.size();
                    create_info.pQueueFamilyIndices = queue_family_indices.data();
                } else {
                    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    create_info.queueFamilyIndexCount = 0;
                    create_info.pQueueFamilyIndices = nullptr;
                }
                create_info.preTransform = swapchain_support.capabilities.currentTransform;
                create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                create_info.presentMode = present_mode;
                create_info.clipped = VK_TRUE;
                create_info.oldSwapchain = nullptr;
                VkSwapchainKHR swapchain;
                if (vkCreateSwapchainKHR(device_->get<VkDevice>(), &create_info, nullptr, &swapchain) != VK_SUCCESS) {
                    throw std::runtime_error("Could not create swapchain!");
                }
                spdlog::info("Successfully created swapchain!");
                return swapchain;
            }
            static void destroy_swapchain(void* object, void* user_arg) {
                auto arg = (swapchain_destroy_arg*)user_arg;
                vkDestroySwapchainKHR(arg->device->get<VkDevice>(), (VkSwapchainKHR)object, nullptr);
                delete arg;
            }
            static vulkan_object::lifetime_descriptor get_desc(std::shared_ptr<vulkan_object> device, std::shared_ptr<vulkan_object> surface, GLFWwindow* window) {
                return {
                    create_swapchain,
                    destroy_swapchain,
                    new swapchain_create_arg{
                        device,
                        surface,
                        window
                    },
                    new swapchain_destroy_arg{
                        device
                    }
                };
            }
            swapchain::swapchain(std::shared_ptr<vulkan_object> device, std::shared_ptr<vulkan_object> surface, GLFWwindow* window) : vulkan_object(get_desc(device, surface, window)) { }
        }
    }
}