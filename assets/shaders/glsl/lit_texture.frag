#version 460 core

struct Material {
    vec4  ambient;
    vec4  diffuse;
    vec4  specular;
    float shine;
};

struct DirectionalLight {
    vec4 direction;

    vec4  ambient;
    vec4  diffuse;
    vec4  specular;
    float attenuation;
};

struct PointLight {
    vec4 position;

    vec4  ambient;
    vec4  diffuse;
    vec4  specular;
    float attenuation;
};

struct SpotLight {
    vec4 position;
    vec4 heading;

    vec4  ambient;
    vec4  diffuse;
    vec4  specular;
    float attenuation;

    float inner_cone;
    float outer_cone;
};

layout(std140, binding = 1) uniform WorldAndMaterial {
    mat4  world_matrix;
    Material material;
};

layout(std140, binding = 2) uniform LightParameters {
    DirectionalLight directional;
    PointLight       point;
    SpotLight        spot;
    vec4             camera_pos;
};

layout(location = 0) in VS_OUT {
    vec4 world_pos;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 texcoords;
} ps_in;

layout (binding = 0) uniform sampler2D diffuse_map;
layout (binding = 1) uniform sampler2D normal_map;

layout(location = 0) out vec4 final_color;

vec4 phong_model(vec3 normal, vec4 light_pos, vec4 ambient, vec4 diffuse,
                 vec4 specular, float attenuation) {
    vec4 surface_to_light  = normalize(light_pos - ps_in.world_pos);
    vec4 surface_to_camera = normalize(camera_pos - ps_in.world_pos);
    
    //ambient
    vec4 ambient_result = ambient * material.ambient;

    //diffuse
    float diffuse_coefficient = max(0.0, dot(normal.xyz, surface_to_light.xyz));
    vec4 diffuse_result = diffuse_coefficient * diffuse;
    
    //specular
    float shine = 0.0;
    if(diffuse_coefficient > 0.0) {
        vec3 reflected = reflect(-surface_to_light.xyz, normal);
        shine = pow(max(0.0, dot(surface_to_camera.xyz, reflected)),
                    material.shine);
    }

    vec4 specular_result = specular * material.specular * shine;
    
    //attenuation
    float to_light = length(light_pos - ps_in.world_pos);
    float attenuation_result = 1.0 / dot(
        vec3(0.0, attenuation, 0.0),
        vec3(1.0, to_light, pow(to_light, 2))
    );

    return ambient_result + attenuation_result *
                            (diffuse_result + specular_result);
}

vec4 directional_calc(vec3 normal) {
    // ambient
    vec4 ambient = directional.ambient * material.ambient;

    // diffuse 
    vec4  direction = normalize(-directional.direction);
    float angle     = max(dot(normal, direction.xyz), 0.0);
    vec4  diffuse   = directional.diffuse * angle * material.diffuse;
    
    // specular
    vec3  view_dir    = normalize(camera_pos.xyz - ps_in.world_pos.xyz);
    vec3  reflect_dir = reflect(-direction.xyz, normal);  
    float shine       = pow(max(dot(view_dir, reflect_dir), 0.0),
                            material.shine);
    vec4  specular    = directional.specular * material.specular * shine;

    return vec4(ambient + diffuse + specular);
}

vec4 point_calc(vec3 normal) {
    return phong_model(normal, point.position, point.ambient, point.diffuse,
                       point.specular, point.attenuation);
}

vec4 spot_calc(vec3 normal) {
    vec4 light_to_surface = normalize(ps_in.world_pos - spot.position);
    float pixel_angle = dot(light_to_surface, spot.heading);

    if(pixel_angle > spot.outer_cone) {
        vec4 light_intensity = phong_model(normal, spot.position, spot.ambient,
                                           spot.diffuse, spot.specular,
                                           spot.attenuation);

        float theta   = pixel_angle;
        float epsilon = spot.inner_cone - spot.outer_cone;
        float fade    = clamp((theta - spot.outer_cone) / epsilon, 0.0, 1.0); 

        return light_intensity * fade;
    }
    else {
        return vec4(0.0);
    }
}

void main() {
    mat3 TBN = mat3(ps_in.tangent,
                    ps_in.bitangent,
                    ps_in.normal);

    vec3 normal = texture(normal_map, ps_in.texcoords).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(TBN * normal);

    // combination!
    vec4 directional = directional_calc(normal);
    vec4 point = point_calc(normal);
    vec4 spot  = spot_calc(normal);

    vec4 light_intensity = directional + point + spot;
    vec4 texel = texture(diffuse_map, ps_in.texcoords);

	final_color = texel * light_intensity;
}
