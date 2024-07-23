#include "Demo.hpp"

#include "brasstacks/platform/vulkan/pipeline/vkShaderStage.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShaderProgram.hpp"

// =============================================================================
Demo::Demo() :
    Application("Brasstacks")
{ }

// =============================================================================
void Demo::init() {
    // btx::vkShaderStage vert("shaders/demo.vert");
    // btx::vkShaderStage frag("shaders/demo.frag");

    btx::vkShaderProgram shader;
    shader.add_stage("shaders/demo.vert")
          .add_stage("shaders/demo.frag")
          .create();
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