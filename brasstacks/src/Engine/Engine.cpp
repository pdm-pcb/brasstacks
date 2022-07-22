#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/Engine.hpp"

#include "brasstacks/Application/TargetWindow.hpp"
#include "brasstacks/Engine/RenderContext.hpp"
#include "brasstacks/Engine/RenderConfig.hpp"
#include "brasstacks/Engine/RenderQueue.hpp"

#include "brasstacks/System/Input/KeyCodes.hpp"
#include "brasstacks/Events/Event.hpp"
#include "brasstacks/Events/KeyboardEvent.hpp"

#include "brasstacks/ECS/ECSView.hpp"
#include "brasstacks/ECS/Systems/CameraSystem.hpp"
#include "brasstacks/ECS/Systems/CubeSystem.hpp"
#include "brasstacks/Cameras/CameraBag.hpp"



#include "brasstacks/Shaders/ShaderFlatColor.hpp"
#include "brasstacks/Meshes/MeshFlatColor.hpp"
#include "brasstacks/Shaders/ShaderFlatTexture.hpp"
#include "brasstacks/Meshes/MeshFlatTexture.hpp"
#include "brasstacks/Shaders/ShaderLitTexture.hpp"
#include "brasstacks/Meshes/MeshLitTexture.hpp"

namespace btx {

std::atomic<bool> w = false;
std::atomic<bool> a = false;
std::atomic<bool> s = false;
std::atomic<bool> d = false;
std::atomic<bool> add_cube = false;

void Engine::on_event(Event &event) {
    switch(event.type()) {
        case EventType::WindowClosed:
            BTX_ENGINE_TRACE("Engine received WindowClosed");
            RenderQueue::shutdown();
            _render_thread_running.store(false);
            _update_thread_running.store(false);
            break;

        case EventType::KeyPressed:
        {
            auto *key_event = reinterpret_cast<KeyPressedEvent *>(&event);
            switch(key_event->key()) {
                case KB_W: w = true; break;
                case KB_A: a = true; break;
                case KB_S: s = true; break;
                case KB_D: d = true; break;

                case KB_SPACE: add_cube = true;
            }
            break;
        }

        case EventType::KeyReleased:
        {
            auto *key_event = reinterpret_cast<KeyReleasedEvent *>(&event);
            switch(key_event->key()) {
                case KB_W: w = false; break;
                case KB_A: a = false; break;
                case KB_S: s = false; break;
                case KB_D: d = false; break;
            }
            break;
        }

        case EventType::MouseMoved:
        {
            auto [ x_offset, y_offset ] =
                reinterpret_cast<MouseMovedEvent *>(&event)->offset();

            auto camera = _ecs->get<cCamera>(CameraBag::get_active());
            camera->pitch -= 0.00125f * y_offset;
            camera->yaw   += 0.00125f * x_offset;

            break;
        }

        default: break;
    }
}

void Engine::_add_cube(ShaderFlatColor *shader) {
    Entity::ID new_cube = _ecs->new_entity();
    _ecs->assign<cCube>(new_cube);

    auto cube_transform = _ecs->assign<cTransform>(new_cube);
    cube_transform->position = {
        _rng(_twister),
        _rng(_twister),
        -25.0f
    };

    auto cube_render    = _ecs->assign<cRender>(new_cube);
    cube_render->shader = shader;
    cube_render->mesh   = new MeshFlatColor(Mesh::Primitives::Cube);

    _ecs->assign<cWorldMat>(new_cube);

    ++_cube_count;
    if(_cube_count % 10 == 0) {
        add_cube = false;
        BTX_ENGINE_TRACE(
            "Adding cube {}: {:.02f},{:.02f},{:.02f}",
            _cube_count,
            cube_transform->position.x,
            cube_transform->position.y,
            cube_transform->position.z
        );
    }
}

void Engine::update_thread() {
    {
        std::unique_lock<std::mutex> lock(_thread_startup);
        _render_thread_ready.wait(
            lock,
            [this]{ return _render_thread_running.load(); }
        );
    }

    _update_thread_running.store(true);
    while(_update_thread_running) {
        RenderQueue::begin_scene();
        Clock::update_tick();

            CameraSystem::update(_ecs, { w, a, s, d }, Clock::frame_delta());
            CubeSystem::update(_ecs, Clock::frame_delta());

            uint32_t submission_count = 0;

            for(const auto id : ECSView<cRender>(*_ecs)) {
                auto render = _ecs->get<cRender>(id);
                RenderQueue::submit(render->shader, id);
                ++submission_count;
            }

        RenderQueue::end_scene();
        Clock::update_tock();
    }
}

void Engine::render_thread() {
    RenderQueue::init();

    _render_context->init();
    _render_context->set_clear_color(0.11f, 0.11f, 0.11f, 1.0f);

    if(RenderConfig::vsync_on) {
        _render_context->set_swap_interval(1);
    }

    // auto shader_fc = new ShaderFlatColor;  // TODO: this belongs elsewhere, too.
    // auto shader_ft = new ShaderFlatTexture;
    auto shader_lt = new ShaderLitTexture;

    Entity::ID floor = _ecs->new_entity();
    _ecs->assign<cTransform>(floor);



    // _ecs->assign<cWorldMat>(floor);
    // auto floor_render    = _ecs->assign<cRender>(floor);
    // floor_render->shader = shader_fc;
    // floor_render->mesh   = new MeshFlatColor(
    //     Mesh::Primitives::XZPlane,
    //     500.0f,
    //     -13.0f
    // );




    // _ecs->assign<cWorldMat>(floor);
    // auto floor_render    = _ecs->assign<cRender>(floor);
    // floor_render->shader = shader_ft;
    // floor_render->mesh   = new MeshFlatTexture(
    //     Mesh::Primitives::XZPlane,
    //     10.0f, 10.0f,
    //     500.0f,
    //     -13.0f
    // );
    // static_cast<MeshFlatTexture *>(floor_render->mesh)->set_texture(
    //     "../../assets/textures/rocky_surface_diffuse.jpg",
    //     false, true,
    //     Texture2D::MinFilter::linear_mipmap_nearest,
    //     Texture2D::MagFilter::linear,
    //     Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    // );




    auto floor_render    = _ecs->assign<cRender>(floor);
    floor_render->shader = shader_lt;
    floor_render->mesh   = new MeshLitTexture(
        Mesh::Primitives::XZPlane,
        10.0f, 10.0f,
        500.0f,
        -13.0f
    );
    static_cast<MeshLitTexture *>(floor_render->mesh)->set_texture(
        "../../assets/textures/rocky_surface_diffuse.jpg",
        "../../assets/textures/rocky_surface_normal.jpg",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    auto phong = _ecs->assign<cPhongNormalMap>(floor);
    phong->world_and_material.ambient  = { 0.05f, 0.05f, 0.05f, 1.0f };
    phong->world_and_material.diffuse  = { 0.75f, 0.75f, 0.75f, 1.0f };
    phong->world_and_material.specular = { 0.85f, 0.85f, 0.85f, 1.0f };
    phong->world_and_material.shine    = 1.0f;

    auto &dir   = phong->light_params.directional_light;
    auto &point = phong->light_params.point_light;
    auto &spot  = phong->light_params.spot_light;

    dir.direction       = glm::normalize(glm::vec4(0.0f, -1.0f, 1.0f, 0.0f));
    dir.props.diffuse   = { 0.5f, 0.5f, 0.5f, 1.0f };
    dir.props.ambient   = dir.props.diffuse * 0.1f;
    dir.props.ambient.w = 1.0f;
    dir.props.specular  = dir.props.diffuse;

    point.position        = { 0.0f, -5.0f, 0.0f, 1.0f };
    point.props.diffuse   = { 0.05f, 0.25f, 0.15f, 1.0f };
    point.props.ambient   = point.props.diffuse * 0.1f;
    point.props.ambient.w = 1.0f;
    point.props.specular  = point.props.diffuse;




    _render_thread_running.store(true);
    _render_thread_ready.notify_one();

    while(_render_thread_running) {
        _render_context->run();

        if(add_cube) {
            // _add_cube(shader_fc);
        }
    }

    // TODO: this reeeeeally belongs somewhere else. As does the creation of
    //       new meshes. Presumably, these operations should live in the
    //       renderer itself, and simple handles should be returned upon
    //       creation.
    for(const auto id : ECSView<cRender>(*_ecs)) {
        auto render = _ecs->get<cRender>(id);
        delete render->mesh;
    }

    _render_context->shutdown();

    // delete shader_fc;
    // delete shader_ft;
    delete shader_lt;
}

Engine::Engine() :
    _render_thread_running { false },
    _update_thread_running { false },
    _render_context { RenderContext::create() },
    _ecs        { new ECS },
    _cube_count { 0 },
    _twister    { _rd() },
    _rng        { -10.0f, 10.0f }

{
    TargetWindow::current()->subscribe_to(this, EventType::WindowClosed);
    TargetWindow::current()->subscribe_to(this, EventType::KeyPressed);
    TargetWindow::current()->subscribe_to(this, EventType::KeyReleased);
    TargetWindow::current()->subscribe_to(this, EventType::MouseMoved);
    TargetWindow::current()->subscribe_to(this, EventType::MouseButtonPressed);
    TargetWindow::current()->subscribe_to(this, EventType::MouseButtonReleased);

    ECS::set_active(_ecs);

    Entity::ID camera = _ecs->new_entity();
    _ecs->assign<cMove>(camera);

    auto camera_tc = _ecs->assign<cTransform>(camera);
    camera_tc->position  = { 0.0f, 0.0f, 2.0f };

    auto camera_cc = _ecs->assign<cCamera>(camera);
    camera_cc->view_matrix = glm::lookAt(
        camera_tc->position,
        camera_cc->forward,
        camera_cc->up
    );
    camera_cc->proj_matrix = glm::perspective(
        math::pi_over_four,
        static_cast<float>(RenderConfig::window_x_res) /
        static_cast<float>(RenderConfig::window_y_res),
        RenderConfig::near_clip, RenderConfig::far_clip
    );

    CameraBag::set_active(camera);
}

Engine::~Engine() {
    TargetWindow::current()->unsubscribe(this, EventType::WindowClosed);
    TargetWindow::current()->unsubscribe(this, EventType::KeyPressed);
    TargetWindow::current()->unsubscribe(this, EventType::KeyReleased);
    TargetWindow::current()->unsubscribe(this, EventType::MouseMoved);
    TargetWindow::current()->unsubscribe(this, EventType::MouseButtonPressed);
    TargetWindow::current()->unsubscribe(this, EventType::MouseButtonReleased);

    delete _render_context;
    delete _ecs;
}

} // namespace btx