#ifndef BRASSTACKS_CORE_STATE_APPSTATEPLAY_HPP
#define BRASSTACKS_CORE_STATE_APPSTATEPLAY_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/core/state/AppStateBase.hpp"

namespace btx {

class Application;

class AppStatePlay final : public AppStateBase {
public:
    explicit AppStatePlay(Application &application);
    ~AppStatePlay() override = default;

    void enter() override;
    void exit() override;
    void execute() override;

    void keyboard_event(KeyboardEvent const &event) override;
    void mouse_button_event(MouseButtonEvent const &) override { }


    AppStatePlay(AppStatePlay &&) = delete;
    AppStatePlay(AppStatePlay const &) = delete;

    AppStatePlay & operator=(AppStatePlay &&) = delete;
    AppStatePlay & operator=(AppStatePlay const &) = delete;

private:
    Application &_application;
};

} // namespace btx

#endif // BRASSTACKS_CORE_STATE_APPSTATEPLAY_HPP