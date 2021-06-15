#include "libvkplayground_pch.h"
#include "application.h"
namespace libplayground {
    namespace vk {
        struct instance_creation_user_arg {
            std::string title;
        };
        static void* create_instance(void* user_arg) {
            auto arg = (instance_creation_user_arg*)user_arg;
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
            uint32_t glfw_extension_count = 0;
            const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
            create_info.enabledExtensionCount = glfw_extension_count;
            create_info.ppEnabledExtensionNames = glfw_extensions;
            create_info.enabledLayerCount = 0;
            VkInstance instance;
            if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
                throw new std::runtime_error("Failed to create instance!");
            }
            delete arg;
            spdlog::info("Successfully created instance!");
            return instance;
        }
        static void destroy_instance(void* object, void*) {
            vkDestroyInstance((VkInstance)object, nullptr);
        }
        application::application(const std::string& title, int32_t width, int32_t height) {
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
            vulkan_object::lifetime_descriptor desc;
            desc.create_proc = create_instance;
            desc.create_user_arg = new instance_creation_user_arg{ this->m_title };
            desc.destroy_proc = destroy_instance;
            desc.destroy_user_arg = nullptr;
            this->m_instance = std::make_shared<vulkan_object>(desc);
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