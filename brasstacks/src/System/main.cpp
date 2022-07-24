#ifndef BRASSTACKS_APPLICATION_ENTRYPOINT_HPP
#define BRASSTACKS_APPLICATION_ENTRYPOINT_HPP

#include "brasstacks/Application/Application.hpp"
#include "brasstacks/Utility/Logger.hpp"

int main(int argc, char **argv) {
    btx::Logger::init();

    for(int i = 0; i < argc; i++) {
        BTX_ENGINE_TRACE("CLI Argument {}: {}", i+1, argv[i]);
    }

    auto *application = new btx::Application;

    if(argc == 1) {
        application->configure(argv[1]);
    }
    else {
        application->configure(nullptr);
    }

    application->run();

    delete application;

    return 0;
}

#endif // BRASSTACKS_APPLICATION_ENTRYPOINT_HPP