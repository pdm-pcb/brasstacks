#include "brasstacks/System/pch.hpp"
#include "brasstacks/Meshes/MeshLitTexture.hpp"

#include "brasstacks/Meshes/VertexBuffer.hpp"
#include "brasstacks/Textures/Texture2D.hpp"

namespace btx {

void MeshLitTexture::bind_vertex_buffer() const {
    _buffer->bind();
    _diffuse->bind(0u);
    _normal->bind(1u);
}

void MeshLitTexture::set_texture(const char *diffuse_filepath,
                                 const char *normal_filepath,
                                 bool flip_vertical, bool gen_mipmaps,
                                 const Texture2D::MinFilter min_filter,
                                 const Texture2D::MagFilter mag_filter,
                                 const Texture2D::Wrap wrap_s,
                                 const Texture2D::Wrap wrap_t)
{
    _diffuse = Texture2D::create(diffuse_filepath, flip_vertical, gen_mipmaps,
                                 min_filter, mag_filter, wrap_s, wrap_t);
    _normal = Texture2D::create(normal_filepath, flip_vertical, gen_mipmaps,
                                min_filter, mag_filter, wrap_s, wrap_t);
}

void MeshLitTexture::_build_cube(const float scale) {
    _vertices = new Vertex[_vertex_count] {
        // front face
        {{ -0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
        {{  0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }},

        // back face
        {{ -0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
        {{  0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
        {{  0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }},

        // left face
        {{ -0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
        {{ -0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},

        // right face
        {{  0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
        {{  0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},

        // bottom face
        {{  0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
        {{ -0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
        {{ -0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }},
        {{  0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }},

        // top face
        {{ -0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
        {{  0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }},
    };

    _faces = new Mesh::Face[_face_count] {
		{  0,  1,  2 }, {  2,  3,  0 }, {  6,  5,  4 }, {  4,  7,  6 },
		{  8,  9, 10 }, { 10, 11,  8 }, { 12, 13, 14 }, { 14, 15, 12 },
		{ 16, 17, 18 }, { 18, 19, 16 }, { 20, 21, 22 }, { 22, 23, 20 }
    };
}

void MeshLitTexture::_build_xzplane(const float scale, const float y_offset,
                                     const float u_repeat, const float v_repeat)
{
    _vertices = new Vertex[_vertex_count] {
        {{ -0.5f * scale,  y_offset,  0.5f * scale, 1.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f * u_repeat, 0.0f * v_repeat }},
        {{  0.5f * scale,  y_offset,  0.5f * scale, 1.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f * u_repeat, 0.0f * v_repeat }},
        {{  0.5f * scale,  y_offset, -0.5f * scale, 1.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f * u_repeat, 1.0f * v_repeat }},
        {{ -0.5f * scale,  y_offset, -0.5f * scale, 1.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f * u_repeat, 1.0f * v_repeat }}
    };

    _faces = new Face[_face_count] {
        { 0, 1, 2 },
        { 2, 3, 0 }
    };
}

void MeshLitTexture::_build_xyplane(const float scale, const float z_offset,
                                     const float u_repeat, const float v_repeat)
{
    _vertices = new Vertex[_vertex_count] {
        {{ -0.5f * scale, -0.5f * scale,  z_offset, 1.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f * u_repeat, 0.0f * v_repeat }},
        {{  0.5f * scale, -0.5f * scale,  z_offset, 1.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f * u_repeat, 0.0f * v_repeat }},
        {{  0.5f * scale,  0.5f * scale,  z_offset, 1.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f * u_repeat, 1.0f * v_repeat }},
        {{ -0.5f * scale,  0.5f * scale,  z_offset, 1.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
          { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f * u_repeat, 1.0f * v_repeat }},
    };

    _faces = new Face[_face_count] {
        { 0, 1, 2 },
        { 2, 3, 0 }
    };
}

void MeshLitTexture::_calc_tbn() {
    for(uint32_t face = 0; face < _face_count; face++) {
        uint32_t a_idx = _faces[face].index_a;
        uint32_t b_idx = _faces[face].index_b;
        uint32_t c_idx = _faces[face].index_c;
 
        glm::vec3 v0  = _vertices[b_idx].position - _vertices[a_idx].position;
        glm::vec2 uv0 = _vertices[b_idx].texcoord - _vertices[a_idx].texcoord;
 
        glm::vec3 v1  = _vertices[c_idx].position - _vertices[a_idx].position;
        glm::vec2 uv1 = _vertices[c_idx].texcoord - _vertices[a_idx].texcoord;
 
        float r = 1.0f / (uv0[0] * uv1[1] - uv1[0] * uv0[1]);
 
        glm::vec3 tangent = (uv1[1] * v0 - uv0[1] * v1) * r;
        tangent = glm::normalize(tangent);
 
        _vertices[a_idx].tangent += glm::vec4(tangent, 0.0f);
        _vertices[b_idx].tangent += glm::vec4(tangent, 0.0f);
        _vertices[c_idx].tangent += glm::vec4(tangent, 0.0f);
 
        glm::vec3 bitangent = (uv0[0] * v1 - uv1[0] * v0) * r;
        bitangent = glm::normalize(bitangent);
 
        _vertices[a_idx].bitangent += glm::vec4(bitangent, 0.0f);
        _vertices[b_idx].bitangent += glm::vec4(bitangent, 0.0f);
        _vertices[c_idx].bitangent += glm::vec4(bitangent, 0.0f);
 
        glm::vec3 normal = glm::cross(v0, v1);
        normal = glm::normalize(normal);
 
        _vertices[a_idx].normal += glm::vec4(normal, 0.0f);
        _vertices[b_idx].normal += glm::vec4(normal, 0.0f);
        _vertices[c_idx].normal += glm::vec4(normal, 0.0f);
    }
 
    for(uint32_t v_idx = 0; v_idx < _vertex_count; v_idx++) {
        _vertices[v_idx].normal    = glm::normalize(_vertices[v_idx].normal);
        _vertices[v_idx].tangent   = glm::normalize(_vertices[v_idx].tangent);
        _vertices[v_idx].bitangent = glm::normalize(_vertices[v_idx].bitangent);
    }
}

MeshLitTexture::MeshLitTexture(const Primitives primitive,
                                 const float u_repeat,
                                 const float v_repeat,
                                 const float scale,
                                 const float plane_offset) :
    _buffer { VertexBuffer::create({
        { "position", VBElement::Type::vec4f, true },
        { "normal",   VBElement::Type::vec4f, true },
        { "tangent",  VBElement::Type::vec4f, true },
        { "binormal", VBElement::Type::vec4f, true },
        { "texcoord", VBElement::Type::vec2f, true }
    })},
    _vertices { nullptr },
    _faces    { nullptr },
    _diffuse  { nullptr },
    _normal   { nullptr },
    _vertex_count { 0 },
    _face_count   { 0 }
{
    switch(primitive) {
        case Primitives::Cube:
            _vertex_count = CUBE_VERTS;
            _face_count = CUBE_FACES;
            _build_cube(scale);
            break;

        case Primitives::XZPlane:
            _vertex_count = PLANE_VERTS;
            _face_count = PLANE_FACES;
            _build_xzplane(scale, plane_offset, u_repeat, v_repeat);
            break;

        case Primitives::XYPlane:
            _vertex_count = PLANE_VERTS;
            _face_count = PLANE_FACES;
            _build_xyplane(scale, plane_offset, u_repeat, v_repeat);
            break;
    }

    _buffer->set_buffer(_vertices, _vertex_count * sizeof(Vertex));
    _buffer->set_indices(_faces, _face_count);
}

MeshLitTexture::~MeshLitTexture() {
    delete _buffer;
    delete _vertices;
    delete _faces;
    delete _diffuse;
    delete _normal;
}

} // namespace btx