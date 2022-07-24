#include "brasstacks/brasstacks.hpp"

void btx::load_resources() {                      
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
}