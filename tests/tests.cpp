#include <catch2/catch_test_macros.hpp>

#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"

using namespace btx;

TEST_CASE("vkInstance create and destroy", "[Vulkan]" ) {

    SECTION("With validation") {
        vkInstance::Config const config {
            .extensions = { },
            .enable_validation = true,
        };
        REQUIRE(vkInstance::create(config, "Unit Tests", BTX_VERSION));
        REQUIRE(vkInstance::destroy());
    }

    SECTION("Without validation") {
        vkInstance::Config const config {
            .extensions = { },
            .enable_validation = false,
        };
        REQUIRE(vkInstance::create(config, "Unit Tests", BTX_VERSION));
        REQUIRE(vkInstance::destroy());
    }
}

TEST_CASE("vkInstance getters", "[Vulkan]") {
    vkInstance::create({ }, "Unit Tests", BTX_VERSION);

    REQUIRE(vkInstance::native() != nullptr);
    REQUIRE(vkInstance::loader().success() == true);

    vkInstance::destroy();
}

TEST_CASE("vkPhysicalDevice populate list", "[Vulkan]") {
    vkInstance::create({ }, "Unit Tests", BTX_VERSION);

    // vkPhysicalDevice::populate_device_list();

    vkInstance::destroy();
}