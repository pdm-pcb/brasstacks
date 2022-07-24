#include "brasstacks/brasstacks.hpp"

#include "cCube.hpp"

void add_cube() {
    btx::ECS *ecs = btx::ECS::get_active();

    for(std::size_t col = 0; col < 1; ++col) {
        for(std::size_t row = 0; row < 1; ++row) {
            btx::Entity::ID new_cube = ecs->new_entity();
            ecs->assign<cCube>(new_cube);

            auto cube_transform = ecs->assign<btx::cTransform>(new_cube);
            cube_transform->position = { 10.0f * col, 0.0f, -10.0f * row -10.0f };
            cube_transform->scale = { 5.0f, 5.0f, 5.0f };

            ecs->assign<btx::cWorldMat>(new_cube);

            auto cube_render    = ecs->assign<btx::cRender>(new_cube);
            cube_render->shader = btx::ShaderLibrary::checkout("lit_texture");
            cube_render->mesh   = btx::MeshLibrary::checkout("lit_texture_cube");

            auto material = ecs->assign<btx::cMaterial>(new_cube);
            material->diffuse_map = btx::TextureLibrary::checkout("stone_diffuse");
            material->normal_map  = btx::TextureLibrary::checkout("stone_normal");
        }
    }
}