#include "Demo.hpp"

#include "brasstacks/platform/vulkan/pipeline/vkShaderProgram.hpp"

// =============================================================================
Demo::Demo() :
    Application("Brasstacks")
{ }

// =============================================================================
void Demo::init() {
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