#ifndef BRASSTACKS_CORE_STATE_APPSTATEPLAY_HPP
#define BRASSTACKS_CORE_STATE_APPSTATEPLAY_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/core/state/AppStateBase.hpp"

namespace btx {

class Application;

class AppStatePlay final : public AppStateBase {
public:
    explicit AppStatePlay(Application &target_window);
    ~AppStatePlay() override = default;

    void enter() override;
    void exit() override;
    void execute() override;

    void key_press(KeyPressEvent const &event) override;
    void mouse_button_press(MouseButtonPressEvent const &) override { }


    AppStatePlay(AppStatePlay &&) = delete;
    AppStatePlay(AppStatePlay const &) = delete;

    AppStatePlay & operator=(AppStatePlay &&) = delete;
    AppStatePlay & operator=(AppStatePlay const &) = delete;

private:
    Application &_application;
};

} // namespace btx

#endif // BRASSTACKS_CORE_STATE_APPSTATEPLAY_HPP