#include "Demo.hpp"

#include "brasstacks/platform/vulkan/pipeline/vkShaderStage.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShaderProgram.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"

// =============================================================================
Demo::Demo() :
    Application("Demo", BTX_VERSION)
{ }

// =============================================================================
void Demo::init() {
    btx::vkShaderStage vert("shaders/demo.vert");
    btx::vkShaderStage frag("shaders/demo.frag");

    btx::vkPipeline pipeline;

    pipeline.add_shader(vert)
            .add_shader(frag)
            .create(btx::vkPipeline::Config{
                .viewport_extent { .width = 1920, .height = 1080 },
                .viewport_offset { .x = 0, .y = 0 }
            });
}

// =============================================================================
void Demo::shutdown() {

}

// =============================================================================
void Demo::update() {
}

// =============================================================================
void Demo::draw() {

}