#pragma once
#include "vulkan_object.h"
namespace libplayground {
    namespace vk {
        namespace vulkan {
            class device : public vulkan_object {
            public:
                device(std::shared_ptr<vulkan_object> instance, bool validation_layers_enabled);
            };
        }
    }
}