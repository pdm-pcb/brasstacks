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
    material->diffuse_map = btx::TextureLibrary::checkout("wood_diffuse");
    material->normal_map  = btx::TextureLibrary::checkout("wood_normal");

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
    material->diffuse_map = btx::TextureLibrary::checkout("tile_diffuse");
    material->normal_map  = btx::TextureLibrary::checkout("tile_normal");

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
    point->props.diffuse     = { 0.5f,  0.5f, 0.5f, 1.0f };
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


    spot->position          = { 25.0f, -2.0f, -35.0f, 1.0f };
    spot->heading           = { glm::normalize(glm::vec3(-spot->position)), 1.0f };
    spot->props.diffuse     = { 0.5f,  0.5f, 0.5f, 1.0f };
    spot->props.ambient     = spot->props.diffuse * 0.1f;
    spot->props.ambient.w   = 1.0f;
    spot->props.specular    = spot->props.diffuse;
    spot->props.attenuation = 0.05f;
    spot->inner_cone = { std::cos(glm::radians(6.0f))  };
    spot->outer_cone = { std::cos(glm::radians(12.0f)) };

    btx::Entity::ID spotcaster = ecs->new_entity();
    auto spotcaster_render = ecs->assign<btx::cRender>(spotcaster);
    spotcaster_render->mesh = btx::MeshLibrary::checkout("flat_color_cube_white");
    spotcaster_render->shader = btx::ShaderLibrary::checkout("flat_color");
    auto spotcaster_tx = ecs->assign<btx::cTransform>(spotcaster);
    spotcaster_tx->position = spot->position;
    spotcaster_tx->scale = { 0.25f, 0.25f, 0.25f };
    auto spotcaster_world = ecs->assign<btx::cWorldMat>(spotcaster);
    spotcaster_world->world_mat =
        glm::translate(btx::mat4_ident, spotcaster_tx->position) *
        glm::scale(btx::mat4_ident, spotcaster_tx->scale);
}

void btx::load_user_resources() {                      
    MeshLibrary::load(
        Mesh::Type::LitTexture, "floor",
        Mesh::Primitives::XZPlane,
        { 1.0f, 1.0f, 1.0f },
        50.0f, 50.0f,
        500.0f,
        -5.0f
    );

    //
    // tile pavement texture
    //
    TextureLibrary::load(
        "../../assets/textures/Pavement_Brick_002d.jpg",
        "tile_diffuse",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Pavement_Brick_002n.jpg",
        "tile_normal",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Pavement_Brick_002s.jpg",
        "tile_specular",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    //
    // asphalt texture
    //
    TextureLibrary::load(
        "../../assets/textures/Asphalt_001d.jpg",
        "asphalt_diffuse",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Asphalt_001n.jpg",
        "asphalt_normal",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Asphalt_001s.jpg",
        "asphalt_specular",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    //
    // concrete texture
    //
    TextureLibrary::load(
        "../../assets/textures/Concrete_001d.png",
        "concrete_diffuse",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Concrete_001n.png",
        "concrete_normal",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Concrete_001s.png",
        "concrete_specular",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    //
    // stone wall texture
    //
    TextureLibrary::load(
        "../../assets/textures/Stone_Wall_002d.jpg",
        "stone_diffuse",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Stone_Wall_002n.jpg",
        "stone_normal",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Stone_Wall_002s.jpg",
        "stone_specular",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    //
    // brick wall texture
    //
    TextureLibrary::load(
        "../../assets/textures/Brick_wall_002d.jpg",
        "brick_diffuse",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Brick_wall_002n.jpg",
        "brick_normal",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Brick_wall_002s.jpg",
        "brick_specular",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    //
    // wood texture
    //
    TextureLibrary::load(
        "../../assets/textures/Wood_White_Cedar_001d.jpg",
        "wood_diffuse",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Wood_White_Cedar_001n.jpg",
        "wood_normal",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    TextureLibrary::load(
        "../../assets/textures/Wood_White_Cedar_001s.jpg",
        "wood_specular",
        false, true,
        Texture2D::MinFilter::linear_mipmap_nearest,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
    );

    Demo::add_floor();
    Demo::add_cube();
}