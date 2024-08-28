#include "Demo.hpp"

#include "brasstacks/platform/vulkan/pipeline/vkShaderStage.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShaderProgram.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"

// =============================================================================
Demo::Demo() :
    Application("Brasstacks")
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

    // btx::vkShaderProgram shader;
    // shader.add_stage("shaders/demo.vert")
    //       .add_stage("shaders/demo.frag")
    //       .create();
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