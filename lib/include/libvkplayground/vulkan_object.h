#pragma once
namespace libplayground {
    namespace vk {
        class vulkan_object {
        public:
            struct lifetime_descriptor {
                std::function<void*(void* user_arg)> create_proc;
                std::function<void(void* object, void* user_arg)> destroy_proc;
                void* create_user_arg;
                void* destroy_user_arg;
            };
            vulkan_object(const lifetime_descriptor& desc);
            template<typename T> T get() {
                return (T)this->m_object;
            }
            ~vulkan_object();
        private:
            void* m_object;
            lifetime_descriptor m_desc;
        };
    }
}