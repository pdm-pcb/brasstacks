#include "brasstacks/brasstacks.hpp"

void add_terrain() {
    btx::ECS *ecs = btx::ECS::get_active();
    btx::Entity::ID floor = ecs->new_entity();
    ecs->assign<btx::cTransform>(floor);
    ecs->assign<btx::cWorldMat>(floor);

    auto floor_render    = ecs->assign<btx::cRender>(floor);
    floor_render->shader = btx::ShaderLibrary::checkout("lit_texture");
    floor_render->mesh   = btx::MeshLibrary::checkout("floor");

    auto material = ecs->assign<btx::cMaterial>(floor);
    material->ambient  = { 0.05f, 0.05f, 0.05f, 1.0f };
    material->diffuse  = { 0.50f, 0.50f, 0.50f, 1.0f };
    material->specular = { 0.75f, 0.75f, 0.75f, 1.0f };
    material->diffuse_map = btx::TextureLibrary::checkout("tile_diffuse");
    material->normal_map  = btx::TextureLibrary::checkout("tile_normal");

    static_cast<btx::ShaderLitTexture *>
        (floor_render->shader)->store_per_frame_id(floor);

    auto phong = ecs->assign<btx::cPhongParams>(floor);
    auto *dir   = &phong->params.directional_light;
    auto *point = &phong->params.point_light;
    auto *spot  = &phong->params.spot_light;

    dir->direction       = glm::normalize(glm::vec4(0.0f, -2.0f, -1.0f, 0.0f));
    dir->props.diffuse   = { 0.25f, 0.25f, 0.25f, 1.0f };
    dir->props.ambient   = dir->props.diffuse * 0.1f;
    dir->props.ambient.w = 1.0f;
    dir->props.specular  = dir->props.diffuse;

    point->position          = { 0.0f, -3.0f, -4.0f, 1.0f };
    point->props.diffuse     = { 0.5f, 0.5f, 0.5f, 1.0f };
    point->props.ambient     = point->props.diffuse * 0.1f;
    point->props.ambient.w   = 1.0f;
    point->props.specular    = point->props.diffuse;
    point->props.attenuation = 0.25f;

    btx::Entity::ID pointcaster = ecs->new_entity();
    auto pointcaster_render = ecs->assign<btx::cRender>(pointcaster);
    pointcaster_render->mesh = btx::MeshLibrary::checkout("flat_color_cube_white");
    pointcaster_render->shader = btx::ShaderLibrary::checkout("flat_color");
    auto pointcaster_tx = ecs->assign<btx::cTransform>(pointcaster);
    pointcaster_tx->position = point->position;
    pointcaster_tx->scale = { 0.25f, 0.25f, 0.25f };
    auto pointcaster_world = ecs->assign<btx::cWorldMat>(pointcaster);
    pointcaster_world->world_mat =
        glm::translate(btx::mat4_ident, pointcaster_tx->position) *
        glm::scale(btx::mat4_ident, { pointcaster_tx->scale });

    spot->position          = { 0.0f, 10.0f, -35.0f, 1.0f };
    spot->heading           = { 0.0f, -1.0f, 0.0f, 1.0f };
    spot->props.diffuse     = { 0.25f, 0.25f, 0.75f, 1.0f };
    spot->props.ambient     = spot->props.diffuse * 0.1f;
    spot->props.ambient.w   = 1.0f;
    spot->props.specular    = spot->props.diffuse;
    spot->props.attenuation = 0.05f;
    spot->inner_cone = { std::cos(glm::radians(6.0f))  };
    spot->outer_cone = { std::cos(glm::radians(12.0f)) };

    btx::Entity::ID spotcaster = ecs->new_entity();
    auto spotcaster_render = ecs->assign<btx::cRender>(spotcaster);
    spotcaster_render->mesh = btx::MeshLibrary::checkout("flat_color_cube_blue");
    spotcaster_render->shader = btx::ShaderLibrary::checkout("flat_color");
    auto spotcaster_tx = ecs->assign<btx::cTransform>(spotcaster);
    spotcaster_tx->position = spot->position;
    spotcaster_tx->scale = { 0.25f, 0.25f, 0.25f };
    auto spotcaster_world = ecs->assign<btx::cWorldMat>(spotcaster);
    spotcaster_world->world_mat =
        glm::translate(btx::mat4_ident, spotcaster_tx->position) *
        glm::scale(btx::mat4_ident, spotcaster_tx->scale);
}