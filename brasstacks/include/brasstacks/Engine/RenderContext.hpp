#ifndef BRASSTACKS_ENGINE_RENDERCONTEXT_HPP
#define BRASSTACKS_ENGINE_RENDERCONTEXT_HPP

#include <cstdint>

namespace btx {

class TargetWindow;

class RenderContext {
public:
    virtual void init()     = 0;
    virtual void run()      = 0;
    virtual void shutdown() = 0;
    
    virtual void set_swap_interval(std::uint8_t interval) = 0;
    virtual void set_clear_color(float r, float g, float b, float a) = 0;

    static RenderContext * create();

    virtual ~RenderContext() = default;
};

} // namespace btx

#endif // BRASSTACKS_ENGINE_RENDERCONTEXT_HPP