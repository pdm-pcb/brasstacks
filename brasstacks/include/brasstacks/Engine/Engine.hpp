#ifndef BRASSTACKS_ENGINE_ENGINE_HPP
#define BRASSTACKS_ENGINE_ENGINE_HPP

#include "brasstacks/Events/EventListener.hpp"

#include <thread>

namespace btx {

class RenderContext;

class Engine : public EventListener {
public:
    void on_event(Event &event) override;

    void renderer();

    Engine();
    ~Engine();

    Engine(const Engine &&) = delete;
    Engine(Engine &)        = delete;

    Engine & operator=(const Engine &&) = delete;
    Engine & operator=(Engine &)        = delete;

private:
    RenderContext *_render_context;

    struct {
        float r;
        float g;
        float b;
        float a;
    } _clear_color;
};

} // namespace btx

#endif // BRASSTACKS_ENGINE_ENGINE_HPP