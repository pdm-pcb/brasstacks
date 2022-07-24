#include "brasstacks/brasstacks.hpp"

void add_camera() {
    auto ecs = btx::ECS::get_active();

    btx::Entity::ID camera = ecs->new_entity();
    ecs->assign<btx::cMove>(camera);

    auto camera_tc = ecs->assign<btx::cTransform>(camera);
    camera_tc->position  = { 0.0f, 0.0f, 2.0f };

    auto camera_cc = ecs->assign<btx::cCamera>(camera);
    camera_cc->view_matrix = glm::lookAt(
        camera_tc->position,
        camera_cc->forward,
        camera_cc->up
    );
    camera_cc->proj_matrix = glm::perspective(
        btx::math::pi_over_four,
        static_cast<float>(btx::RenderConfig::window_x_res) /
        static_cast<float>(btx::RenderConfig::window_y_res),
        btx::RenderConfig::near_clip, btx::RenderConfig::far_clip
    );

    btx::CameraBag::set_active(camera);
}