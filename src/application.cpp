#include "pch.h"
#include "application.h"
namespace vkplayground {
    application::application(const std::string& title, int32_t width, int32_t height) {
        this->m_window = std::make_shared<window>(title, width, height);
    }
    application::~application() { }
    void application::run() {
        this->load();
        this->main_loop();
        this->unload();
    }
    void application::update() { }
    void application::main_loop() {
        while (!this->m_window->should_close()) {
            this->update();
            this->render();
            this->m_window->swap_buffers();
            window::poll_events();
        }
    }
}