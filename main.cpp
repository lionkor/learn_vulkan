#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <cstring>

#include "types.h"
#include "debug.h"

constexpr u32 WIDTH = 800;
constexpr u32 HEIGHT = 600;

const std::vector<const char*> validation_layers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enable_validation_layers = false;
#else
constexpr bool enable_validation_layers = true;
#endif

class HelloTriangleApp
{
private:
    VkPhysicalDevice m_physical_device { VK_NULL_HANDLE };
    GLFWwindow* m_window { nullptr };
    VkInstance m_instance;
    VkDevice m_device;
    VkQueue m_graphics_queue;

    void init_window() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Window", nullptr, nullptr);
    }

    void init_vulkan() {
        create_instance();
        pick_physical_device();
        create_logical_device();
    }

    void create_logical_device() {
        QueueFamilyIndices indices = find_queue_families(m_physical_device);
        VkDeviceQueueCreateInfo queue_create_info {};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = indices.graphics_family.value();
        queue_create_info.queueCount = 1;
        float queue_priority = 1.0f;
        queue_create_info.pQueuePriorities = &queue_priority;

        VkPhysicalDeviceFeatures device_features {}; // default means everything is VK_FALSE

        VkDeviceCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pQueueCreateInfos = &queue_create_info;
        create_info.queueCreateInfoCount = 1;
        create_info.pEnabledFeatures = &device_features;

        if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device");
        }
        vkGetDeviceQueue(m_device, indices.graphics_family.value(), 0, &m_graphics_queue);
    }

    void pick_physical_device() {
        u32 device_count = 0;
        vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
        if (device_count == 0) {
            throw std::runtime_error("failed to find GPU with Vulkan support");
        }
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());
        for (const auto& device : devices) {
            if (is_device_suitable(device)) {
                m_physical_device = device;
                break;
            }
        }

        if (m_physical_device == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU");
        }
    }

    bool is_device_suitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = find_queue_families(device);
        VkPhysicalDeviceProperties device_props;
        vkGetPhysicalDeviceProperties(device, &device_props);
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceFeatures(device, &device_features);
        return device_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
               && indices.is_complete();
    }

    struct QueueFamilyIndices {
        std::optional<u32> graphics_family;

        bool is_complete() {
            return graphics_family.has_value();
        }
    };

    QueueFamilyIndices find_queue_families(VkPhysicalDevice device) {
        QueueFamilyIndices indices;
        u32 family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_families(family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, queue_families.data());
        u32 i = 0;
        for (const auto& queue_family : queue_families) {
            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphics_family = i;
            }
            if (indices.is_complete()) {
                break;
            }
            ++i;
        }
        return indices;
    }

    void create_instance() {
        if (enable_validation_layers && !check_validation_layer_support()) {
            throw std::runtime_error("requested validation layers not available");
        }
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
        if (enable_validation_layers) {
            create_info.enabledLayerCount = static_cast<u32>(validation_layers.size());
            create_info.ppEnabledLayerNames = validation_layers.data();
        } else {
            create_info.enabledLayerCount = 0;
        }

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
        vkDestroyDevice(m_device, nullptr);
        vkDestroyInstance(m_instance, nullptr);

        glfwDestroyWindow(m_window);
        glfwTerminate();
        m_window = nullptr;
    }

    bool check_validation_layer_support() {
        u32 layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
        for (const char* layer_name : validation_layers) {
            bool layer_found = false;
            for (const auto& layer_properties : available_layers) {
                if (std::strcmp(layer_name, layer_properties.layerName) == 0) {
                    layer_found = true;
                    break;
                }
            }
            if (!layer_found) {
                return false;
            }
        }
        return true;
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
