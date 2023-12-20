/**
 * @file Application.hpp
 * @brief The base class for any application using the library.
 */

#ifndef BRASSTACKS_SYSTEM_APPLICATION_HPP
#define BRASSTACKS_SYSTEM_APPLICATION_HPP

#include "brasstacks/pch.hpp"

namespace btx {

struct KeyReleaseEvent;
class TargetWindow;
class Renderer;

/**
 * @brief The base class for any application using the library.
 *
 * Application must be inherited from and instantiated in the user's entry
 * point. The constructor will bring up platform systems (including TargetWindow
 * and Renderer). The user must then call Application::run(), which will call
 * the overridden Application::init() and proceed to the main loop. Cleanup
 * is handled after the main loop exits, with Application::shutdown() being
 * called automatically before the destructor.
 */
class Application {
public:
    /**
     * @brief Construct the Application object.
     * @param app_name User-provided name which is applied to window titles,
     * driver hints, and so on.
     */
    explicit Application(std::string_view const app_name);
    virtual ~Application();

    /**
     * @brief The main applicaiton loop.
     */
    void run();

    /**
     * @brief Key event handler for Application.
     * @param event Which key was released
     */
    void on_key_release(KeyReleaseEvent const &event);

    Application() = delete;

    Application(Application &&) = delete;
    Application(const Application &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(const Application &) = delete;

protected:
    /**
     * @brief A chance for the user to do their setup.
     *
     * The overridden function is called after other systems are online, just
     * before the main application loop begins.
     */
    virtual void init(Renderer &renderer) = 0;
    /**
     * @brief A chance for the user to do any cleanup.
     *
     * The overridden function is called before other systems are taken down,
     * just after the main application loop ends.
     */
    virtual void shutdown() = 0;
    /**
     * @brief A chance for the user to do necessary processing.
     *
     * The overridden function is called once per frame. The user should call
     * Application::request_draw() to submit draw calls to the renderer here.
     */
    virtual void update() = 0;

    /**
     * @brief An interface between the user and Renderer.
     *
     * The user must call this function in order to submit draw calls to the
     * Renderer backend.
     */
    void request_draw();

private:
    /**
     * @brief Controls whether the application's main loop continues.
     */
    bool _running;

    /**
     * @brief The platform window to which Renderer will draw and from which
     * Application will take input.
     */
    TargetWindow *_target_window;

    /**
     * @brief The renderer backend capable of parsing draw calls and juggling
     * swapchain images and pipelines.
     */
    Renderer *_renderer;
};

} // namespace btx

#endif // BRASSTACKS_SYSTEM_APPLICATION_HPP