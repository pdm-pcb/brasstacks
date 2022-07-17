#ifndef BRASSTACKS_ENGINE_ENGINE_HPP
#define BRASSTACKS_ENGINE_ENGINE_HPP

#include "brasstacks/Events/EventListener.hpp"

#include <atomic>
#include <mutex>
#include <condition_variable>

namespace btx {

class RenderContext;
class ShaderFlatColor;
class MeshFlatColor;
class TransformSystem;

class Engine : public EventListener {
public:
    void on_event(Event &event) override;

    void update_thread();
    void render_thread();

    Engine();
    ~Engine();

    Engine(const Engine &&) = delete;
    Engine(Engine &)        = delete;

    Engine & operator=(const Engine &&) = delete;
    Engine & operator=(Engine &)        = delete;

private:
    std::mutex _thread_startup;
    std::atomic<bool> _render_thread_running;
    std::atomic<bool> _update_thread_running;

    std::condition_variable _render_thread_ready;
    std::condition_variable _update_thread_ready;

    RenderContext *_render_context;

    struct {
        float r;
        float g;
        float b;
        float a;
    } _clear_color;

    ShaderFlatColor *_shader;
    MeshFlatColor   *_mesh01;
    MeshFlatColor   *_mesh02;
    TransformSystem *_transforms;
};

} // namespace btx

#endif // BRASSTACKS_ENGINE_ENGINE_HPP