#ifndef BRASSTACKS_CORE_STATE_APPSTATEMENU_HPP
#define BRASSTACKS_CORE_STATE_APPSTATEMENU_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/core/state/AppStateBase.hpp"

namespace btx {

class AppStateMenu final : public AppStateBase {
public:
    AppStateMenu() : AppStateBase(AppState::MENU_STATE) { }
    ~AppStateMenu() override = default;

    void enter()   override { }
    void exit()    override { }
    void execute() override;

    void keyboard_event(KeyboardEvent const &event) override;
    void mouse_button_event(MouseButtonEvent const &event) override;

    AppStateMenu(AppStateMenu &&) = delete;
    AppStateMenu(AppStateMenu const &) = delete;

    AppStateMenu & operator=(AppStateMenu &&) = delete;
    AppStateMenu & operator=(AppStateMenu const &) = delete;
};

} // namespace btx

#endif // BRASSTACKS_CORE_STATE_APPSTATEMENU_HPP