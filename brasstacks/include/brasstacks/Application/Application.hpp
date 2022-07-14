#ifndef BRASSTACKS_APPLICATION_APPLICATION_HPP
#define BRASSTACKS_APPLICATION_APPLICATION_HPP

#include "brasstacks/Events/EventListener.hpp"

#include <atomic>

namespace btx {
	
class Application : public EventListener {
public:
    void on_event(Event &event) override;

    // TODO: Implement save-and-load conf files
    //       JSON, true plaintext, or just dump the RenderConfig object?
    //       Probably at least plaintext to aid in not breaking conf files
    //       when new fields are added. Plus who knows how the data might be
    //       packed. =)
    void configure(const char *conf_filename);
    void run();

    Application();
    virtual ~Application();

private:
    std::atomic<bool> _running;
};

Application * create_application();

} // namespace btx

#endif // BRASSTACKS_APPLICATION_APPLICATION_HPP