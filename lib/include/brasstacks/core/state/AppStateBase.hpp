#ifndef BRASSTACKS_CORE_STATE_APPSTATEBASE_HPP
#define BRASSTACKS_CORE_STATE_APPSTATEBASE_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/core/state/StateBase.hpp"

namespace btx {

struct KeyPressEvent;
struct MouseButtonPressEvent;

class AppStateBase : public StateBase {
public:
    virtual ~AppStateBase() = default;

    virtual void key_press(KeyPressEvent const &event) = 0;
    virtual void mouse_button_press(MouseButtonPressEvent const &event) = 0;

protected:
    AppStateBase() = default;
};

} // namespace btx

#endif // BRASSTACKS_CORE_STATE_APPSTATEBASE_HPP