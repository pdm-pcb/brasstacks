#ifndef BRASSTACKS_CORE_STATE_APPSTATEBASE_HPP
#define BRASSTACKS_CORE_STATE_APPSTATEBASE_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/core/state/StateBase.hpp"

namespace btx {

struct KeyboardEvent;
struct MouseButtonEvent;

class AppStateBase : public StateBase {
public:
    AppStateBase() = delete;
    virtual ~AppStateBase() override = default;

    auto type() const { return _state_type; }

    virtual void keyboard_event(KeyboardEvent const &event) = 0;
    virtual void mouse_button_event(MouseButtonEvent const &event) = 0;

protected:
    explicit AppStateBase(AppState const state_type) :
        _state_type { state_type }
    { }

private:
    AppState const _state_type;
};

} // namespace btx

#endif // BRASSTACKS_CORE_STATE_APPSTATEBASE_HPP