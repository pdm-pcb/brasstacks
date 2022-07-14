#ifndef BRASSTACKS_APPLICATION_TARGETWINDOW_HPP
#define BRASSTACKS_APPLICATION_TARGETWINDOW_HPP

#include "brasstacks/Events/EventPublisher.hpp"

#include <cstdint>

namespace btx {

class Event;
class RenderContext;

class TargetWindow : public EventPublisher {
public:
    virtual void init()     = 0;
    virtual void run()      = 0;
    virtual void shutdown() = 0;

    virtual void * get_native() const = 0;

    static TargetWindow * create();
    static TargetWindow * current()               { return _current;   }
    static void set_current(TargetWindow *window) { _current = window; }

    virtual ~TargetWindow() = default;

    TargetWindow(const TargetWindow &&) = delete;
    TargetWindow(TargetWindow &)        = delete;

    TargetWindow & operator=(const TargetWindow &&) = delete;
    TargetWindow & operator=(TargetWindow &)        = delete;

protected:
    static TargetWindow *_current;

    TargetWindow() = default;
};

} // namespace btx

#endif // BRASSTACKS_APPLICATION_TARGETWINDOW_HPP