#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "types.h"
#include "debug.h"

constexpr u32 WIDTH = 800;
constexpr u32 HEIGHT = 600;

class HelloTriangleApp
{
private:
    GLFWwindow* m_window { nullptr };
    VkInstance m_instance;

    void init_window() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Window", nullptr, nullptr);
    }

    void init_vulkan() {
        create_instance();
    }

    void create_instance() {
        VkApplicationInfo app_info {};
        // optional data
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "Hello Triangle";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "No Engine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;

        // mandatory
        VkInstanceCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;
        // configure extensions
        u32 glfw_extension_count = 0;
        const char** glfw_extensions = nullptr;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        create_info.enabledExtensionCount = glfw_extension_count;
        create_info.ppEnabledExtensionNames = glfw_extensions;
        // this is 0 for now, idk why
        create_info.enabledLayerCount = 0;

        VkResult result = vkCreateInstance(&create_info, nullptr, &m_instance);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan instance");
        }
    }

    void main_loop() {
        while (!glfwWindowShouldClose(m_window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        m_window = nullptr;
    }

public:
    void run() {
        init_window();
        init_vulkan();
        main_loop();
        cleanup();
    }
};

int main() {
    HelloTriangleApp app;
    try {
        app.run();
    } catch (const std::exception& e) {
        debug("error: " << e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
