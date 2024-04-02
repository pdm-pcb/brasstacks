#ifndef DEMO_HPP
#define DEMO_HPP

#include "brasstacks/core/Application.hpp"

#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/assets/libraries/MeshLibrary.hpp"
#include "brasstacks/assets/libraries/TextureLibrary.hpp"

#include "brasstacks/math/Mat4.hpp"

class Demo final : public btx::Application {
public:
    Demo();
    ~Demo() override = default;

    void init() override;
    void shutdown() override;

    void update() override;
    void record_commands() const override;

    void destroy_swapchain_resources() override;
    void create_swapchain_resources() override;

private:
    std::unique_ptr<btx::vkPipeline> _pipeline;

    btx::MeshLibrary::MeshIter _plane_mesh;
    btx::math::Mat4            _plane_mat;

    btx::MeshLibrary::MeshIter _cube_mesh;
    btx::math::Mat4            _cube_mat;

    btx::TextureLibrary::TextureIter _texture;
};

#endif // DEMO_HPP