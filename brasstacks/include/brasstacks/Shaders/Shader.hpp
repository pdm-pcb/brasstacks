#ifndef BRASSTACKS_SHADERS_SHADER_HPP
#define BRASSTACKS_SHADERS_SHADER_HPP

#include "brasstacks/System/glm.hpp"

#include <cstdint>

namespace btx {

constexpr std::size_t MAX_POINT_LIGHTS { 128u };
constexpr std::size_t MAX_SPOT_LIGHTS  { 128u };

struct RenderComp;

class Shader {
public:
    enum class Type {
        Vertex,
        Fragment,
        Geometry
    };

    struct CameraBufferData {
        glm::mat4 view;
        glm::mat4 projection;
    };

    struct WorldAndMaterial {
        glm::mat4 world;
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        float     shine;
    };

    struct LightProperties {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        float shine;
        float attenuation;
    };

    struct DirectionalLight {       
        glm::vec4 direction;
        LightProperties props;
    };
                                    
    struct PointLight {
        glm::vec4 position;
        LightProperties props;
    };

    struct SpotLight {
        glm::vec4 position;
        glm::vec4 heading;
        LightProperties props;
        float inner_cone;
        float outer_cone;
    };

    struct LightParameters {
        DirectionalLight directional_light;
        std::array<PointLight, MAX_POINT_LIGHTS> point_light;
        std::array<SpotLight,  MAX_SPOT_LIGHTS>  spot_light;

        glm::vec4 camera_pos;

        uint32_t  point_count;
        uint32_t  spot_count;
    };

    static char * load_source(const char *path);

    virtual void bind()   const = 0;
    virtual void unbind() const = 0;
    
    virtual void update_camera(const glm::mat4 &view,
                               const glm::mat4 &projection) const = 0;

    virtual void update_render_data(const RenderComp &render_c) const = 0;

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