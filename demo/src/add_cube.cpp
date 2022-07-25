#include "brasstacks/brasstacks.hpp"

#include "cCube.hpp"

void add_cubes(const float offset, const std::size_t cubes) {
    btx::ECS *ecs = btx::ECS::get_active();

    for(std::size_t col = 0; col < cubes; ++col) {
        for(std::size_t row = 0; row < cubes; ++row) {
            btx::Entity::ID new_cube = ecs->new_entity();
            ecs->assign<cCube>(new_cube);

            auto cube_transform = ecs->assign<btx::cTransform>(new_cube);
            cube_transform->position = { offset * col, 0.0f, -10.0f * row - 15.0f };
            cube_transform->scale = { 5.0f, 5.0f, 5.0f };

            ecs->assign<btx::cWorldMat>(new_cube);

            auto cube_render    = ecs->assign<btx::cRender>(new_cube);
            cube_render->shader = btx::ShaderLibrary::checkout("lit_texture");
            cube_render->mesh   = btx::MeshLibrary::checkout("lit_texture_cube");

            auto material = ecs->assign<btx::cMaterial>(new_cube);
            if(col % 2 == 0) {
                if(row % 2 == 0) {
                    material->diffuse_map = btx::TextureLibrary::checkout("stone_diffuse");
                    material->normal_map  = btx::TextureLibrary::checkout("stone_normal");
                }
                else {
                    material->diffuse_map = btx::TextureLibrary::checkout("concrete_diffuse");
                    material->normal_map  = btx::TextureLibrary::checkout("concrete_normal");
                }
            }
            else {
                if(row % 2 == 0) {
                    material->diffuse_map = btx::TextureLibrary::checkout("wood_diffuse");
                    material->normal_map  = btx::TextureLibrary::checkout("wood_normal");
                }
                else {
                    material->diffuse_map = btx::TextureLibrary::checkout("brick_diffuse");
                    material->normal_map  = btx::TextureLibrary::checkout("brick_normal");
                }
            }

        }
    }
}