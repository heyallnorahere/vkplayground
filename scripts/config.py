import platform
import sys
import subprocess
import os, os.path
import scripts.dependencies as deps
class PlatformPkgConfig:
    def __init__(self, package_install_command: list[str], package_spec: deps.PackageSpec):
        self.package_install_command = package_install_command
        self.package_spec = package_spec
    def install(self):
        args = list[str]()
        for arg in self.package_install_command:
            args.append(arg)
        for group in self.package_spec.package_groups:
            args_copy = args.copy()
            args_copy.append(group)
            subprocess.call(args_copy)
        for pkg in self.package_spec.individual_packages:
            args.append(pkg)
        subprocess.call(args)
packages = {
    "Arch-Linux": PlatformPkgConfig(["sudo", "pacman", "-Syq"], deps.ARCH_PACKAGES),
    "Ubuntu": PlatformPkgConfig(["sudo", "apt-get", "install", "-y"], deps.UBUNTU_PACKAGES)
}
def get_platform_name():
    verbose_platform_name = platform.system()
    if verbose_platform_name == "Linux":
        process = subprocess.Popen(["uname", "-a"], stdout=subprocess.PIPE)
        output = process.communicate()
        verbose_platform_name = output[0].decode("utf-8")
    platform_names = {
        "-arch": "Arch-Linux",
        "-Ubuntu": "Ubuntu"
    }
    for keyword in platform_names:
        if keyword in verbose_platform_name:
            return platform_names[keyword]
    return "unknown"
def install_packages(platform_name: str):
    try:
        pkg_config = packages[platform_name]
        print("Installing packages...")
        pkg_config.install()
        return 0
    except KeyError:
        print("The platform ({}) is not supported!")
        return -1
def configure(args: list[str]):
    cwd = os.getcwd()
    cmake_args = [
        "cmake",
        cwd,
        "-B",
        os.path.join(cwd, "build")
    ]
    for arg in args:
        cmake_args.append(arg)
    return subprocess.call(cmake_args)
def run(args: list[str]):
    platform_name = get_platform_name()
    return_code = install_packages(platform_name)
    if return_code != 0:
        return return_code
    print("Configuring CMake")
    print("Compiling for: {}".format(platform_name))
    print("Source directory: {}".format(os.getcwd()))
    return_code = configure(args)
    if return_code == 0:
        print("Successfully configured!")
    else:
        print("Configuration failed! Return code: {}".format(return_code))
    return return_code
def main():
    args = list[str]()
    if len(sys.argv) > 1:
        for index in range(1, len(sys.argv)):
            args.append(sys.argv[index])
    run(args)
if __name__ == "__main__":
    main()