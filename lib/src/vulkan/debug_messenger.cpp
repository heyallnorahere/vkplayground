#include "libvkplayground_pch.h"
#include "libvkplayground/vulkan_objects.h"
#include "libvkplayground/debug.h"
namespace libplayground {
    namespace vk {
        namespace vulkan {
            struct debug_messenger_create_arg {
                bool validation_layers_enabled;
                std::shared_ptr<vulkan_object> instance;
            };
            struct debug_messenger_destroy_arg {
                std::shared_ptr<vulkan_object> instance;
            };
            static void* create(void* user_arg) {
                auto arg = (debug_messenger_create_arg*)user_arg;
                if (!arg->validation_layers_enabled) {
                    delete arg;
                    return nullptr;
                }
                VkDebugUtilsMessengerCreateInfoEXT create_info;
                debug::populate_VkDebugUtilsMessengerCreateInfoEXT(create_info);
                auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(arg->instance->get<VkInstance>(), "vkCreateDebugUtilsMessengerEXT");
                if (!vkCreateDebugUtilsMessengerEXT) {
                    delete arg;
                    throw std::runtime_error("Could not load debug messenger creation function!");
                }
                VkDebugUtilsMessengerEXT debug_messenger;
                if (vkCreateDebugUtilsMessengerEXT(arg->instance->get<VkInstance>(), &create_info, nullptr, &debug_messenger) != VK_SUCCESS) {
                    delete arg;
                    throw std::runtime_error("Could not create debug messenger!");
                }
                delete arg;
                spdlog::info("Successfully created debug messenger!");
                return debug_messenger;
            }
            static void destroy(void* object, void* user_arg) {
                auto arg = (debug_messenger_destroy_arg*)user_arg;
                auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(arg->instance->get<VkInstance>(), "vkDestroyDebugUtilsMessengerEXT");
                if (vkDestroyDebugUtilsMessengerEXT && object) {
                    vkDestroyDebugUtilsMessengerEXT(arg->instance->get<VkInstance>(), (VkDebugUtilsMessengerEXT)object, nullptr);
                }
                delete arg;
            }
            static vulkan_object::lifetime_descriptor get_desc(bool validation_layers_enabled, std::shared_ptr<vulkan_object> instance) {
                return {
                    create,
                    destroy,
                    new debug_messenger_create_arg{
                        validation_layers_enabled,
                        instance
                    },
                    new debug_messenger_destroy_arg{
                        instance
                    }
                };
            }
            debug_messenger::debug_messenger(bool validation_layers_enabled, std::shared_ptr<vulkan_object> instance) : vulkan_object(get_desc(validation_layers_enabled, instance)) { }
        }
    }
}