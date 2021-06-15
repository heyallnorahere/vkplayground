#pragma once
namespace libplayground {
    namespace vk {
        class window {
        public:
            window(const std::string& title, int32_t width, int32_t height);
            ~window();
            GLFWwindow* get_pointer();
            bool should_close();
            void swap_buffers();
            static void poll_events();
        private:
            GLFWwindow* m_window;
            int32_t m_width, m_height;
        };
    }
}