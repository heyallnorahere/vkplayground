#include "pch.h"
#include "window.h"
static int window_count = 0;
// for glfw callbacks
static std::unordered_map<GLFWwindow*, vkplayground::window*> window_map;
namespace vkplayground {
    window::window(const std::string& title, int32_t width, int32_t height) {
        if (window_count <= 0) {
            if (!glfwInit()) {
                exit(EXIT_FAILURE);
            }
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        this->m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        this->m_width = width;
        this->m_height = height;
        window_map[this->m_window] = this;
        window_count++;
    }
    window::~window() {
        window_count--;
        glfwDestroyWindow(this->m_window);
        if (window_count <= 0) {
            glfwTerminate();
        }
    }
    GLFWwindow* window::get_pointer() {
        return this->m_window;
    }
    bool window::should_close() {
        return glfwWindowShouldClose(this->m_window);
    }
    void window::swap_buffers() {
        glfwSwapBuffers(this->m_window);
    }
    void window::poll_events() {
        glfwPollEvents();
    }
}