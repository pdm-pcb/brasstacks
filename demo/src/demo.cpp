#include "brasstacks/brasstacks.hpp"
#include "brasstacks/Application/EntryPoint.hpp"

#include "CubeSystem.hpp"

class Demo : public btx::Application {
public:
    static void add_cube();
    static void add_floor();

    Demo()  { BTX_TRACE("User constructor"); }
    ~Demo() { BTX_TRACE("User destructor");  }

private:
    static std::size_t _cube_count;
    static bool _add_cube;
};

std::size_t Demo::_cube_count = 0;
bool Demo::_add_cube = false;

btx::Application * btx::create_application() {
    return new Demo();
}

void btx::user_render_code() {

}

void btx::user_update_code() {
    CubeSystem::update(btx::Clock::frame_delta());
}

void Demo::add_cube() {
    btx::ECS *ecs = btx::ECS::get_active();
    btx::Entity::ID new_cube = ecs->new_entity();
    ecs->assign<btx::cCube>(new_cube);

    auto cube_transform = ecs->assign<btx::cTransform>(new_cube);
    cube_transform->position = { 0.0f, 0.0f, -15.0f };
    cube_transform->scale = { 5.0f, 5.0f, 5.0f };

    ecs->assign<btx::cWorldMat>(new_cube);

    auto cube_render    = ecs->assign<btx::cRender>(new_cube);
    cube_render->shader = btx::ShaderLibrary::checkout("lit_texture");
    cube_render->mesh   = btx::MeshLibrary::checkout("lit_texture_cube");

    auto material = ecs->assign<btx::cMaterial>(new_cube);
    material->diffuse_map = btx::TextureLibrary::checkout("brickwall_diffuse");
    material->normal_map  = btx::TextureLibrary::checkout("brickwall_normal");

    ++_cube_count;
    _add_cube = false;
}

void Demo::add_floor() {
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
    material->diffuse_map = btx::TextureLibrary::checkout("rocky_surface_diffuse");
    material->normal_map  = btx::TextureLibrary::checkout("rocky_surface_normal");

    static_cast<btx::ShaderLitTexture *>
        (floor_render->shader)->store_per_frame_id(floor);

    auto phong = ecs->assign<btx::cPhongParams>(floor);
    auto *dir   = &phong->params.directional_light;
    auto *point = &phong->params.point_light;
    auto *spot  = &phong->params.spot_light;

    dir->direction       = glm::normalize(glm::vec4(0.0f, -2.0f, -1.0f, 0.0f));
    dir->props.diffuse   = { 0.5f, 0.5f, 0.5f, 1.0f };
    dir->props.ambient   = dir->props.diffuse * 0.1f;
    dir->props.ambient.w = 1.0f;
    dir->props.specular  = dir->props.diffuse;

    point->position          = { 0.0f, -3.0f, 0.0f, 1.0f };
    point->props.diffuse     = { 0.0f,  0.0f, 1.0f, 1.0f };
    point->props.ambient     = point->props.diffuse * 0.1f;
    point->props.ambient.w   = 1.0f;
    point->props.specular    = point->props.diffuse;
    point->props.attenuation = 0.25f;

    spot->position          = { 25.0f, -2.0f, -35.0f, 1.0f };
    spot->heading           = { glm::normalize(glm::vec3(-spot->position)), 1.0f };
    spot->props.diffuse     = { 0.0f, 1.0f, 0.0f, 1.0f };
    spot->props.ambient     = spot->props.diffuse * 0.1f;
    spot->props.ambient.w   = 1.0f;
    spot->props.specular    = spot->props.diffuse;
    spot->props.attenuation = 0.05f;
    spot->inner_cone = { std::cos(glm::radians(6.0f))  };
    spot->outer_cone = { std::cos(glm::radians(12.0f)) };
}

void btx::load_user_resources() {                      
    MeshLibrary::load(Mesh::Type::LitTexture, "floor",
                      Mesh::Primitives::XZPlane,
                      10.0f, 10.0f,
                      500.0f,
                      -5.0f
    );

    TextureLibrary::load(
        "../../assets/textures/rocky_surface_diffuse.jpg",
        "rocky_surface_diffuse",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/rocky_surface_normal.jpg",
        "rocky_surface_normal",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/brickwall_diffuse.jpg",
        "brickwall_diffuse",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/brickwall_normal.jpg",
        "brickwall_normal",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Wood_025_basecolor.jpg",
        "wood_025_diffuse",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Wood_025_normal.jpg",
        "wood_025_normal",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    Demo::add_floor();
    Demo::add_cube();
}