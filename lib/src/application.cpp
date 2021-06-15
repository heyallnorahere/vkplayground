#include "libvkplayground_pch.h"
#include "application.h"
static std::vector<const char*> validation_layers = {
    "VK_LAYER_KHRONOS_validation"  
};
namespace libplayground {
    namespace vk {
        struct instance_creation_user_arg {
            std::string title;
            bool validation_layers_enabled;
        };
        struct debug_messenger_creation_user_arg {
            bool validation_layers_enabled;
            std::shared_ptr<vulkan_object> instance;
        };
        struct debug_messenger_destruction_user_arg {
            std::shared_ptr<vulkan_object> instance;
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
            for (const char* layer_name : validation_layers) {
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
        static void* create_instance(void* user_arg) {
            auto arg = (instance_creation_user_arg*)user_arg;
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
            if (arg->validation_layers_enabled) {
                create_info.enabledLayerCount = (uint32_t)validation_layers.size();
                create_info.ppEnabledLayerNames = validation_layers.data();
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
        static void destroy_instance(void* object, void*) {
            vkDestroyInstance((VkInstance)object, nullptr);
        }
        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
            std::string message = "Vulkan: " + std::string(callback_data->pMessage);
            switch (message_severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                spdlog::info(message);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                spdlog::warn(message);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                throw std::runtime_error(message);
                break;
            }
            return VK_FALSE;
        }
        static void* create_debug_messenger(void* user_arg) {
            auto arg = (debug_messenger_creation_user_arg*)user_arg;
            if (!arg->validation_layers_enabled) {
                delete arg;
                return nullptr;
            }
            VkDebugUtilsMessengerCreateInfoEXT create_info{ };
            create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            create_info.pfnUserCallback = debug_callback;
            create_info.pUserData = nullptr;
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
            return debug_messenger;
        }
        static void destroy_debug_messenger(void* object, void* user_arg) {
            auto arg = (debug_messenger_destruction_user_arg*)user_arg;
            auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(arg->instance->get<VkInstance>(), "vkDestroyDebugUtilsMessengerEXT");
            if (vkDestroyDebugUtilsMessengerEXT && object) {
                vkDestroyDebugUtilsMessengerEXT(arg->instance->get<VkInstance>(), (VkDebugUtilsMessengerEXT)object, nullptr);
            }
            delete arg;
        }
        application::application(const std::string& title, int32_t width, int32_t height) {
            this->m_validation_layers_enabled = 
            #ifdef NDEBUG
            false;
            #else
            true;
            #endif
            this->m_window = std::make_shared<window>(title, width, height);
            this->m_title = title;
        }
        application::~application() { }
        void application::run() {
            spdlog::info("Initializing application \"" + this->m_title + "\"...");
            this->init_vulkan();
            this->load();
            spdlog::info("Successfully initialized application: \"" + this->m_title + "\"");
            this->main_loop();
            this->unload();
        }
        void application::update() { }
        void application::init_vulkan() {
            this->m_instance = std::make_shared<vulkan_object>(vulkan_object::lifetime_descriptor{
                create_instance, // create_proc
                destroy_instance, // destroy_proc
                new instance_creation_user_arg{
                    this->m_title,
                    this->m_validation_layers_enabled
                }, // create_user_arg
                nullptr // destroy_user_arg
            });
            this->m_debug_messenger = std::make_shared<vulkan_object>(vulkan_object::lifetime_descriptor{
                create_debug_messenger,
                destroy_debug_messenger,
                new debug_messenger_creation_user_arg{
                    this->m_validation_layers_enabled,
                    this->m_instance
                },
                new debug_messenger_destruction_user_arg{
                    this->m_instance
                }
            });
        }
        void application::main_loop() {
            while (!this->m_window->should_close()) {
                this->update();
                this->render();
                this->m_window->swap_buffers();
                window::poll_events();
            }
        }
    }
}