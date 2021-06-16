#include "libvkplayground_pch.h"
#include "libvkplayground/application.h"
#include "libvkplayground/debug.h"
#include "libvkplayground/vulkan_objects.h"
namespace libplayground {
    namespace vk {
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
            this->m_instance = std::make_shared<vulkan::instance>(this->m_title, this->m_validation_layers_enabled);
            this->m_surface = std::make_shared<vulkan::surface>(this->m_window->get_pointer(), this->m_instance);
            this->m_debug_messenger = std::make_shared<vulkan::debug_messenger>(this->m_validation_layers_enabled, this->m_instance);
            this->m_device = std::make_shared<vulkan::device>(this->m_instance, this->m_surface, this->m_validation_layers_enabled);
        }
        void application::main_loop() {
            while (!this->m_window->should_close()) {
                this->update();
                this->render();
                // todo: swap buffers
                window::poll_events();
            }
        }
    }
}