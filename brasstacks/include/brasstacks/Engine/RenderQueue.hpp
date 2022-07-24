#ifndef BRASSTACKS_ENGINE_RENDERQUEUE_HPP
#define BRASSTACKS_ENGINE_RENDERQUEUE_HPP

#include "brasstacks/ECS/Entity.hpp"

#include <vector>
#include <utility>
#include <mutex>
#include <atomic>
#include <condition_variable>

constexpr std::size_t RENDERQUEUE_PREALLOC = 100;

namespace btx {

class Shader;

class RenderQueue final {
public:
    using ShaderIndex = std::pair<const Shader *, std::size_t>;
    using Queue       = std::vector<Entity::ID>;

    static void begin_scene();
    static void end_scene();

    static void begin_draw();
    static void end_draw();

    static void submit(const Shader *shader, const Entity::ID id);

    static void init();
    static void shutdown();

    static std::vector<ShaderIndex> & get_indices() { return *_shaders_front; }
    static Queue & get_queue(std::size_t index);

    RenderQueue() = delete;
    ~RenderQueue() = default;

    RenderQueue(const RenderQueue &&) = delete;
    RenderQueue(RenderQueue &)        = delete;

    RenderQueue & operator=(const RenderQueue &&) = delete;
    RenderQueue & operator=(RenderQueue &)        = delete;

private:
    static std::vector<ShaderIndex> _shaders_a;
    static std::vector<Queue>       _meshes_a;

    static std::vector<ShaderIndex> _shaders_b;
    static std::vector<Queue>       _meshes_b;

    static std::vector<ShaderIndex> *_shaders_front;
    static std::vector<Queue>       *_meshes_front;

    static std::vector<ShaderIndex> *_shaders_back;
    static std::vector<Queue>       *_meshes_back;

    static std::mutex _front_mutex;
    static std::mutex _back_mutex;

    static std::condition_variable _back_ready;
    static bool _back_empty;

    static bool _running;

    static void _clear_front();
    static void _clear_back();
    static void _swap_queues();
};

} // namespace btx

#endif // BRASSTACKS_ENGINE_RENDERQUEUE_HPP