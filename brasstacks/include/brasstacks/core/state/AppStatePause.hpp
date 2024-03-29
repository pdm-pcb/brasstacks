#ifndef BRASSTACKS_CORE_STATE_APPSTATEPAUSE_HPP
#define BRASSTACKS_CORE_STATE_APPSTATEPAUSE_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/core/state/AppStateBase.hpp"

namespace btx {

class Application;

class AppStatePause final : public AppStateBase {
public:
    explicit AppStatePause(Application &target_window);
    ~AppStatePause() override = default;

    void enter() override;
    void exit() override;
    void execute() override { }

    void keyboard_event(KeyboardEvent const &) override { }
    void mouse_button_event(MouseButtonEvent const &) override { }

    AppStatePause(AppStatePause &&) = delete;
    AppStatePause(AppStatePause const &) = delete;

    AppStatePause & operator=(AppStatePause &&) = delete;
    AppStatePause & operator=(AppStatePause const &) = delete;

private:
    Application &_application;
};

} // namespace btx

#endif // BRASSTACKS_CORE_STATE_APPSTATEPLAY_HPP