#ifndef BRASSTACKS_APPLICATION_ENTRYPOINT_HPP
#define BRASSTACKS_APPLICATION_ENTRYPOINT_HPP

#include "brasstacks/Utility/memtracker.hpp"
#include "brasstacks/System/pch.hpp"

int main(int argc, char **argv) {
    btx::Logger::init();
    btx::Profiler::init();

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

    btx::Profiler::shutdown();

    return 0;
}

#endif // BRASSTACKS_APPLICATION_ENTRYPOINT_HPP