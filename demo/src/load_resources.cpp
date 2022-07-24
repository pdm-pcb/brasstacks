#include "brasstacks/brasstacks.hpp"

#include "brasstacks/Platform/GL/GLContextWGL.hpp"

// TODO: this is suuuuuuuuuuuuuch a hack... but hey, stuff loads quicker

void btx::load_resources() {                      
    MeshLibrary::load(
        Mesh::Type::LitTexture, "floor",
        Mesh::Primitives::XZPlane,
        { 1.0f, 1.0f, 1.0f },
        50.0f, 50.0f,
        500.0f,
        -5.0f
    );

    dynamic_cast<GLContextWGL *>(RenderContext::active())->release_context();

    //
    // tile pavement texture
    //
    std::thread tex_tile_d([] {
        TextureLibrary::load(
            "../../assets/textures/Pavement_Brick_002d.jpg",
            "tile_diffuse",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    std::thread tex_tile_n([] {
        TextureLibrary::load(
            "../../assets/textures/Pavement_Brick_002n.jpg",
            "tile_normal",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    std::thread tex_tile_s([] {
        TextureLibrary::load(
            "../../assets/textures/Pavement_Brick_002s.jpg",
            "tile_specular",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    //
    // asphalt texture
    //
    std::thread tex_asphalt_d([] {
        TextureLibrary::load(
            "../../assets/textures/Asphalt_001d.jpg",
            "asphalt_diffuse",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    std::thread tex_asphalt_n([] {
        TextureLibrary::load(
            "../../assets/textures/Asphalt_001n.jpg",
            "asphalt_normal",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    std::thread tex_asphalt_s([] {
        TextureLibrary::load(
            "../../assets/textures/Asphalt_001s.jpg",
            "asphalt_specular",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    //
    // concrete texture
    //
    std::thread tex_concrete_d([] {
        TextureLibrary::load(
            "../../assets/textures/Concrete_001d.png",
            "concrete_diffuse",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    std::thread tex_concrete_n([] {
        TextureLibrary::load(
            "../../assets/textures/Concrete_001n.png",
            "concrete_normal",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    std::thread tex_concrete_s([] {
        TextureLibrary::load(
            "../../assets/textures/Concrete_001s.png",
            "concrete_specular",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    //
    // stone wall texture
    //
    std::thread tex_stone_d([] {
        TextureLibrary::load(
            "../../assets/textures/Stone_Wall_002d.jpg",
            "stone_diffuse",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    std::thread tex_stone_n([] {
        TextureLibrary::load(
            "../../assets/textures/Stone_Wall_002n.jpg",
            "stone_normal",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    std::thread tex_stone_s([] {
        TextureLibrary::load(
            "../../assets/textures/Stone_Wall_002s.jpg",
            "stone_specular",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    //
    // brick wall texture
    //
    std::thread tex_brick_d([] {
        TextureLibrary::load(
            "../../assets/textures/Brick_wall_002d.jpg",
            "brick_diffuse",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    std::thread tex_brick_n([] {
        TextureLibrary::load(
            "../../assets/textures/Brick_wall_002n.jpg",
            "brick_normal",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    std::thread tex_brick_s([] {
        TextureLibrary::load(
            "../../assets/textures/Brick_wall_002s.jpg",
            "brick_specular",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    //
    // wood texture
    //
    std::thread tex_wood_d([] {
        TextureLibrary::load(
            "../../assets/textures/Wood_White_Cedar_001d.jpg",
            "wood_diffuse",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    std::thread tex_wood_n([] {
        TextureLibrary::load(
            "../../assets/textures/Wood_White_Cedar_001n.jpg",
            "wood_normal",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    std::thread tex_wood_s([] {
        TextureLibrary::load(
            "../../assets/textures/Wood_White_Cedar_001s.jpg",
            "wood_specular",
            false, true,
            Texture2D::MinFilter::linear_mipmap_nearest,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::repeat, Texture2D::Wrap::repeat
        );}
    );

    tex_tile_d.join();
    tex_tile_n.join();
    tex_tile_s.join();

    tex_asphalt_d.join();
    tex_asphalt_n.join();
    tex_asphalt_s.join();

    tex_concrete_d.join();
    tex_concrete_n.join();
    tex_concrete_s.join();

    tex_stone_d.join();
    tex_stone_n.join();
    tex_stone_s.join();

    tex_brick_d.join();
    tex_brick_n.join();
    tex_brick_s.join();

    tex_wood_d.join();
    tex_wood_n.join();
    tex_wood_s.join();

    dynamic_cast<GLContextWGL *>(RenderContext::active())->make_current();
}