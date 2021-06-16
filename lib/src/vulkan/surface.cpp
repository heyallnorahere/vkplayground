#include "libvkplayground_pch.h"
#include "libvkplayground/vulkan_objects.h"
#include "libvkplayground/debug.h"
namespace libplayground {
    namespace vk {
        namespace vulkan {
            struct surface_create_arg {
                GLFWwindow* window;
                std::shared_ptr<vulkan_object> instance;
            };
            struct surface_destroy_arg {
                std::shared_ptr<vulkan_object> instance;
            };
            static void* create_surface(void* user_arg) {
                auto arg = (surface_create_arg*)user_arg;
                VkSurfaceKHR surface;
                if (glfwCreateWindowSurface(arg->instance->get<VkInstance>(), arg->window, nullptr, &surface) != VK_SUCCESS) {
                    delete arg;
                    throw std::runtime_error("Could not create window surface!");
                }
                delete arg;
                spdlog::info("Successfully created surface!");
                return surface;
            }
            static void destroy_surface(void* object, void* user_arg) {
                auto arg = (surface_destroy_arg*)user_arg;
                vkDestroySurfaceKHR(arg->instance->get<VkInstance>(), (VkSurfaceKHR)object, nullptr);
                delete arg;
            }
            static vulkan_object::lifetime_descriptor get_desc(GLFWwindow* window, std::shared_ptr<vulkan_object> instance) {
                return {
                    create_surface,
                    destroy_surface,
                    new surface_create_arg{
                        window,
                        instance
                    },
                    new surface_destroy_arg{
                        instance
                    }
                };
            }
            surface::surface(GLFWwindow* window, std::shared_ptr<vulkan_object> instance) : vulkan_object(get_desc(window, instance)) { }
        }
    }
}