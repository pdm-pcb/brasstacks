#ifndef BRASSTACKS_SHADERS_SHADER_HPP
#define BRASSTACKS_SHADERS_SHADER_HPP

#include "brasstacks/System/glm.hpp"
#include "brasstacks/ECS/Entity.hpp"

#include <cstdint>
#include <array>

namespace btx {

class Shader {
public:
    enum class Type {
        Vertex,
        Pixel,
        Geometry
    };

    struct CameraBufferData {
        glm::mat4 view       { 1.0f };
        glm::mat4 projection { 1.0f };
    };

    struct WorldAndMaterial {
        glm::mat4 world_mat { 0.0f };
        glm::vec4 ambient   { 0.0f };
        glm::vec4 diffuse   { 0.0f };
        glm::vec4 specular  { 0.0f };
        float     shine     { 1.0f };
    };

    struct LightProperties {
        glm::vec4 ambient  { 0.0f };
        glm::vec4 diffuse  { 0.0f };
        glm::vec4 specular { 0.0f };
        float shine        { 1.0f };
        float attenuation  { 1.0f };
    };

    struct alignas(16) DirectionalLight {       
        glm::vec4 direction { 0.0f };
        LightProperties props;
    };
                                    
    struct alignas(16) PointLight {
        glm::vec4 position { 0.0f };
        LightProperties props;
    };

    struct alignas(16) SpotLight {
        glm::vec4 position { 0.0f };
        glm::vec4 heading  { forward_vector, 0.0f };
        LightProperties props;
        float inner_cone { std::cos(glm::radians(6.0f))  };
        float outer_cone { std::cos(glm::radians(12.0f)) };
    };

    struct LightParameters {
        DirectionalLight directional_light;
        PointLight point_light;
        SpotLight spot_light;

        glm::vec4 camera_pos { origin };
    };

    static char * load_source(const char *path);

    virtual void bind()   const = 0;
    virtual void unbind() const = 0;
    
    virtual void update_camera(const glm::mat4 &view,
                               const glm::mat4 &projection) const = 0;

    virtual void update_render_data(const Entity::ID id) const = 0;

    virtual ~Shader() = default;

    Shader(const Shader &&) = delete;
    Shader(Shader &)        = delete;

    Shader & operator=(const Shader &&) = delete;
    Shader & operator=(Shader &)        = delete;

protected:
    Shader() = default;
};

} // namespace btx

#endif // BRASSTACKS_SHADERS_SHADER_HPP