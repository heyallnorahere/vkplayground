class PackageSpec:
    def __init__(self, package_groups: list[str], individual_packages: list[str]):
        self.individual_packages = individual_packages
        self.package_groups = package_groups
ARCH_PACKAGES = PackageSpec(["X11"], [
    "vulkan-extra-layers",
    "vulkan-extra-tools",
    "vulkan-headers",
    "vulkan-tools",
    "vulkan-validation-layers",
    "libxxf86vm",
    "cmake"
])
UBUNTU_PACKAGES = PackageSpec([], [
    "vulkan-tools",
    "libvulkan-dev",
    "vulkan-validationlayers-dev",
    "spirv-tools",
    "libxrandr-dev",
    "libxinerama-dev",
    "libxcursor-dev",
    "libxi-dev",
    "libgl1-mesa-dev",
    "libxxf86vm-dev",
    "cmake"
])