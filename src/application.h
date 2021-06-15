#pragma once
#include "window.h"
namespace vkplayground {
    class application {
    public:
        application(const std::string& title, int32_t width, int32_t height);
        virtual ~application();
        void run();
    protected:
        virtual void load() = 0;
        virtual void update();
        virtual void render() = 0;
        virtual void unload() = 0;
    private:
        void main_loop();
        std::shared_ptr<window> m_window;
    };
}