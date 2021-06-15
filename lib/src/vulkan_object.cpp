#include "libvkplayground_pch.h"
#include "vulkan_object.h"
namespace libplayground {
    namespace vk {
        vulkan_object::vulkan_object(const lifetime_descriptor& desc) {
            this->m_desc = desc;
            if (!this->m_desc.create_proc) {
                throw new std::runtime_error("Creation procedure cannot be null!");
            }
            this->m_object = this->m_desc.create_proc(this->m_desc.create_user_arg);
        }
        vulkan_object::~vulkan_object() {
            if (!this->m_desc.destroy_proc) {
                spdlog::warn("No destroy procedure - possible memory leak");
            } else {
                this->m_desc.destroy_proc(this->m_object, this->m_desc.destroy_user_arg);
            }
        }
    }
}