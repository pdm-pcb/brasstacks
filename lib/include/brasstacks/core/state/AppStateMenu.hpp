#ifndef BRASSTACKS_CORE_STATE_APPSTATEMENU_HPP
#define BRASSTACKS_CORE_STATE_APPSTATEMENU_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/core/state/AppStateBase.hpp"

namespace btx {

class Application;

class AppStateMenu final : public AppStateBase {
public:
    explicit AppStateMenu(Application &application);
    ~AppStateMenu() = default;

    void enter() override;
    void exit() override;
    void execute() override;

    void key_press(KeyPressEvent const &event) override;
    void mouse_button_press(MouseButtonPressEvent const &event) override;


    AppStateMenu(AppStateMenu &&) = delete;
    AppStateMenu(AppStateMenu const &) = delete;

    AppStateMenu & operator=(AppStateMenu &&) = delete;
    AppStateMenu & operator=(AppStateMenu const &) = delete;

private:
    Application &_application;
};

} // namespace btx

#endif // BRASSTACKS_CORE_STATE_APPSTATEMENU_HPP