#include "Demo.hpp"

#include "brasstacks/platform/vulkan/pipeline/vkShader.hpp"

// =============================================================================
Demo::Demo() :
    Application("Brasstacks")
{ }

// =============================================================================
void Demo::init() {
    btx::vkShader vert;
    vert.create("shaders/demo.vert");
    btx::vkShader frag;
    frag.create("shaders/demo.frag");
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