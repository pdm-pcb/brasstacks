#ifndef BRASSTACKS_ENGINE_RENDERCONTEXT_HPP
#define BRASSTACKS_ENGINE_RENDERCONTEXT_HPP

#include <cstdint>

namespace btx {

class TargetWindow;

class RenderContext {
public:
    virtual void init() = 0;
    virtual void shutdown() = 0;
    virtual void swap_buffers() = 0;
    virtual void set_swap_interval(uint8_t interval) = 0;

    static RenderContext * create(const TargetWindow *window);

    virtual ~RenderContext() = default;
};

} // namespace btx

#endif // BRASSTACKS_ENGINE_RENDERCONTEXT_HPP