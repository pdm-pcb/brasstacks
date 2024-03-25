#ifndef BRASSTACKS_CORE_STATE_APPSTATEPLAY_HPP
#define BRASSTACKS_CORE_STATE_APPSTATEPLAY_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/core/state/AppStateBase.hpp"

namespace btx {

class TargetWindow;

class AppStatePlay final : public AppStateBase {
public:
    explicit AppStatePlay(TargetWindow &target_window);
    ~AppStatePlay() = default;

    void enter() override;
    void exit() override;
    void execute() override;

    void key_press(KeyPressEvent const &event) override;
    void mouse_button_press(MouseButtonPressEvent const &event) { }


    AppStatePlay(AppStatePlay &&) = delete;
    AppStatePlay(AppStatePlay const &) = delete;

    AppStatePlay & operator=(AppStatePlay &&) = delete;
    AppStatePlay & operator=(AppStatePlay const &) = delete;

private:
    TargetWindow &_target_window;
};

} // namespace btx

#endif // BRASSTACKS_CORE_STATE_APPSTATEPLAY_HPP