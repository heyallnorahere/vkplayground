#include "libvkplayground_pch.h"
#include "libvkplayground/vulkan_objects.h"
#include "libvkplayground/debug.h"
namespace libplayground {
    namespace vk {
        namespace vulkan {
            struct instance_create_arg {
                std::string title;
                bool validation_layers_enabled;
            };
            static bool check_extensions(const std::vector<const char*>& required_extensions) {
                uint32_t available_count = 0;
                vkEnumerateInstanceExtensionProperties(nullptr, &available_count, nullptr);
                std::vector<VkExtensionProperties> extensions(available_count);
                vkEnumerateInstanceExtensionProperties(nullptr, &available_count, extensions.data());
                for (const char* name : required_extensions) {
                    bool found = false;
                    for (const auto& extension : extensions) {
                        if (strcmp(name, extension.extensionName) == 0) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        return false;
                    }
                }
                return true;
            }
            static bool check_validation_layers() {
                uint32_t layer_count = 0;
                vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
                std::vector<VkLayerProperties> available_layers(layer_count);
                vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
                for (const char* layer_name : debug::validation_layers) {
                    bool found = false;
                    for (const auto& layer : available_layers) {
                        if (strcmp(layer_name, layer.layerName) == 0) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        return false;
                    }
                }
                return true;
            }
            static std::vector<const char*> get_required_extensions(bool validation_layers_enabled) {
                uint32_t glfw_extension_count = 0;
                const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
                std::vector<const char*> extensions(glfw_extensions, glfw_extensions + (size_t)glfw_extension_count);
                if (validation_layers_enabled) {
                    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                }
                return extensions;
            }
            static void* create(void* user_arg) {
                auto arg = (instance_create_arg*)user_arg;
                if (arg->validation_layers_enabled && !check_validation_layers()) {
                    throw std::runtime_error("Validation layers requested, but not available!");
                }
                VkApplicationInfo app_info{ };
                app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                app_info.pApplicationName = arg->title.c_str();
                app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
                app_info.pEngineName = "libvkplayground";
                app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
                app_info.apiVersion = VK_API_VERSION_1_0;
                VkInstanceCreateInfo create_info{ };
                create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                create_info.pApplicationInfo = &app_info;
                auto required_extensions = get_required_extensions(arg->validation_layers_enabled);
                if (!check_extensions(required_extensions)) {
                    delete arg;
                    throw std::runtime_error("The required extensions are not available!");
                }
                create_info.enabledExtensionCount = (uint32_t)required_extensions.size();
                create_info.ppEnabledExtensionNames = required_extensions.data();
                VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
                if (arg->validation_layers_enabled) {
                    create_info.enabledLayerCount = (uint32_t)debug::validation_layers.size();
                    create_info.ppEnabledLayerNames = debug::validation_layers.data();
                    debug::populate_VkDebugUtilsMessengerCreateInfoEXT(debug_create_info);
                    create_info.pNext = &debug_create_info;
                } else {
                    create_info.enabledLayerCount = 0;
                }
                VkInstance instance;
                if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
                    delete arg;
                    throw std::runtime_error("Failed to create instance!");
                }
                delete arg;
                spdlog::info("Successfully created instance!");
                return instance;
            }
            static void destroy(void* object, void*) {
                vkDestroyInstance((VkInstance)object, nullptr);
            }
            static vulkan_object::lifetime_descriptor get_desc(const std::string& title, bool validation_layers_enabled) {
                return {
                    create,
                    destroy,
                    new instance_create_arg{
                        title,
                        validation_layers_enabled
                    },
                    nullptr
                };
            }
            instance::instance(const std::string& title, bool validation_layers_enabled) : vulkan_object(get_desc(title, validation_layers_enabled)) { }
        }
    }
}