#ifndef BRASSTACKS_APPLICATION_TARGETWINDOW_HPP
#define BRASSTACKS_APPLICATION_TARGETWINDOW_HPP

#include "brasstacks/Events/EventPublisher.hpp"

#include <cstdint>

namespace btx {

class Event;
class RenderContext;

class TargetWindow : public EventPublisher {
public:
    virtual void run() = 0;

    virtual void * get_native() const = 0;

    static TargetWindow * create();

    virtual ~TargetWindow() = default;

    TargetWindow(const TargetWindow &&) = delete;
    TargetWindow(TargetWindow &)        = delete;

    TargetWindow & operator=(const TargetWindow &&) = delete;
    TargetWindow & operator=(TargetWindow &)        = delete;

protected:
    TargetWindow() = default;
};

} // namespace btx

#endif // BRASSTACKS_APPLICATION_TARGETWINDOW_HPP