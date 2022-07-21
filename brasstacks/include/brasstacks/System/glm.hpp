#ifndef BRASSTACKS_SYSTEM_GLM_HPP
#define BRASSTACKS_SYSTEM_GLM_HPP

#define GLM_FORCE_CXX17
#define GLM_FORCE_INLINE
#define GLM_FORCE_SSE42
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace btx {

const auto mat4_ident     = glm::mat4 { 1.0f };
const auto up_vector      = glm::vec3 { 0.0f, 1.0f,  0.0f };
const auto right_vector   = glm::vec3 { 1.0f, 0.0f,  0.0f };
const auto forward_vector = glm::vec3 { 0.0f, 0.0f, -1.0f };

} // namespace btx

#endif // BRASSTACKS_SYSTEM_GLM_HPP