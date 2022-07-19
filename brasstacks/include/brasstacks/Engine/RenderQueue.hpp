#ifndef BRASSTACKS_ENGINE_RENDERQUEUE_HPP
#define BRASSTACKS_ENGINE_RENDERQUEUE_HPP

#include "brasstacks/ECS/Entity.hpp"

#include <vector>
#include <utility>
#include <mutex>
#include <atomic>
#include <condition_variable>

// TODO: The various threads need improved synchronization. For example, the
//       program can crash somewhere in the RenderQueue during shutdown. Not
//       sure if it's during render or update, but it's happened more than
//       once.

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

    static std::vector<ShaderIndex> & get_indices() { return _shaders; }
    static Queue get_queue(std::size_t index);

    RenderQueue() = delete;
    ~RenderQueue() = default;

    RenderQueue(const RenderQueue &&) = delete;
    RenderQueue(RenderQueue &)        = delete;

    RenderQueue & operator=(const RenderQueue &&) = delete;
    RenderQueue & operator=(RenderQueue &)        = delete;

private:
    static std::vector<ShaderIndex> _shaders;
    static std::vector<Queue>       _meshes;

    static std::mutex _queue_lock;
    static std::mutex _submission_lock;

    static std::condition_variable _queue_ready;
    static bool _queue_empty;

    static void _clear_queue();
};

} // namespace btx

#endif // BRASSTACKS_ENGINE_RENDERQUEUE_HPP