#ifndef BRASSTACKS_ENGINE_ENGINE_HPP
#define BRASSTACKS_ENGINE_ENGINE_HPP

#include "brasstacks/Events/EventListener.hpp"

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <random>

#include "brasstacks/ECS/ECS.hpp"

namespace btx {

class RenderContext;

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
    std::mutex        _thread_startup;
    std::atomic<bool> _render_thread_running;
    std::atomic<bool> _update_thread_running;

    std::condition_variable _render_thread_ready;
    std::condition_variable _update_thread_ready;

    RenderContext *_render_context;
    ECS *_ecs;
};

void load_user_resources();
void user_update_code();
void user_render_code();

} // namespace btx

#endif // BRASSTACKS_ENGINE_ENGINE_HPP