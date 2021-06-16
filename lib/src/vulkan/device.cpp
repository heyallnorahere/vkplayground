#include "libvkplayground_pch.h"
#include "libvkplayground/vulkan_objects.h"
#include "libvkplayground/debug.h"
namespace libplayground {
    namespace vk {
        namespace vulkan {
            struct device_create_arg {
                std::shared_ptr<vulkan_object> instance, surface;
                bool validation_layers_enabled;
            };
            struct queue_family_indices {
                std::optional<uint32_t> graphics_family, present_family;
                bool is_complete() {
                    return this->graphics_family.has_value() && this->present_family.has_value();
                }
            };
            static queue_family_indices find_queue_families(VkPhysicalDevice device, std::shared_ptr<vulkan_object> surface) {
                queue_family_indices indices;
                uint32_t queue_family_count = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
                std::vector<VkQueueFamilyProperties> queue_families((size_t)queue_family_count);
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
                for (uint32_t i = 0; i < (uint32_t)queue_families.size(); i++) {
                    if (indices.is_complete()) {
                        break;
                    }
                    VkBool32 present_support = false;
                    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface->get<VkSurfaceKHR>(), &present_support);
                    if (present_support) {
                        indices.present_family = i;
                    }
                    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                        indices.graphics_family = i;
                    }
                }
                return indices;
            }
            std::vector<const char*> device_extensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };
            bool check_device_extension_support(VkPhysicalDevice device) {
                uint32_t extension_count = 0;
                vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
                std::vector<VkExtensionProperties> available_extensions((size_t)extension_count);
                vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());
                std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());
                for (const auto& extension : available_extensions) {
                    required_extensions.erase(extension.extensionName);
                }
                return required_extensions.empty();
            }
            static bool is_device_suitable(VkPhysicalDevice device, std::shared_ptr<vulkan_object> surface) {
                queue_family_indices indices = find_queue_families(device, surface);
                bool extensions_supported = check_device_extension_support(device);
                return indices.is_complete() && extensions_supported;
            }
            static uint32_t rate_device(VkPhysicalDevice device, std::shared_ptr<vulkan_object> surface) {
                uint32_t score = 0;
                VkPhysicalDeviceProperties device_properties;
                vkGetPhysicalDeviceProperties(device, &device_properties);
                if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                    score += 1000;
                }
                score += device_properties.limits.maxImageDimension2D;
                VkPhysicalDeviceFeatures device_features;
                vkGetPhysicalDeviceFeatures(device, &device_features);
                if (!device_features.geometryShader || !is_device_suitable(device, surface)) {
                    return 0;
                }
                return score;
            }
            static VkPhysicalDevice pick_device(std::shared_ptr<vulkan_object> instance, std::shared_ptr<vulkan_object> surface, std::function<void()> on_throw) {
                VkPhysicalDevice physical_device = nullptr;
                uint32_t device_count = 0;
                vkEnumeratePhysicalDevices(instance->get<VkInstance>(), &device_count, nullptr);
                if (device_count == 0) {
                    on_throw();
                    throw std::runtime_error("Could not find any GPUs with Vulkan support!");
                }
                std::vector<VkPhysicalDevice> devices(device_count);
                vkEnumeratePhysicalDevices(instance->get<VkInstance>(), &device_count, devices.data());
                std::multimap<uint32_t, VkPhysicalDevice> candidates;
                for (const auto& device : devices) {
                    int score = rate_device(device, surface);
                    candidates.insert(std::make_pair(score, device));
                }
                auto it = candidates.rbegin();
                if (it->first > 0) {
                    physical_device = it->second;
                } else {
                    on_throw();
                    throw std::runtime_error("Could not find a suitable GPU!");
                }
                return physical_device;
            }
            static void* create_device(void* user_arg) {
                auto arg = (device_create_arg*)user_arg;
                std::function<void()> on_throw = [&]() {
                    delete arg;
                };
                VkPhysicalDevice physical_device = pick_device(arg->instance, arg->surface, on_throw);
                queue_family_indices indices = find_queue_families(physical_device, arg->surface);
                VkDeviceQueueCreateInfo queue_create_info{ };
                queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_create_info.queueFamilyIndex = indices.graphics_family.value();
                queue_create_info.queueCount = 1;
                float queue_priority = 1.f;
                queue_create_info.pQueuePriorities = &queue_priority;
                VkPhysicalDeviceFeatures features{ };
                // todo: populate "features" with required device features
                VkDeviceCreateInfo create_info{ };
                create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                create_info.pQueueCreateInfos = &queue_create_info;
                create_info.queueCreateInfoCount = 1;
                create_info.pEnabledFeatures = &features;
                create_info.enabledExtensionCount = (uint32_t)device_extensions.size();
                create_info.ppEnabledExtensionNames = device_extensions.data();
                if (arg->validation_layers_enabled) {
                    create_info.enabledLayerCount = (uint32_t)debug::validation_layers.size();
                    create_info.ppEnabledLayerNames = debug::validation_layers.data();
                } else {
                    create_info.enabledLayerCount = 0;
                }
                VkDevice device;
                if (vkCreateDevice(physical_device, &create_info, nullptr, &device) != VK_SUCCESS) {
                    delete arg;
                    throw std::runtime_error("Failed to create logical device!");
                }
                delete arg;
                spdlog::info("Successfully created logical device!");
                return device;
            }
            static void destroy_device(void* object, void*) {
                vkDestroyDevice((VkDevice)object, nullptr);
            }
            static vulkan_object::lifetime_descriptor get_desc(std::shared_ptr<vulkan_object> instance, std::shared_ptr<vulkan_object> surface, bool validation_layers_enabled) {
                return {
                    create_device,
                    destroy_device,
                    new device_create_arg{
                        instance,
                        surface,
                        validation_layers_enabled
                    },
                    nullptr
                };
            }
            device::device(std::shared_ptr<vulkan_object> instance, std::shared_ptr<vulkan_object> surface, bool validation_layers_enabled) : vulkan_object(get_desc(instance, surface, validation_layers_enabled)) { }
        }
    }
}