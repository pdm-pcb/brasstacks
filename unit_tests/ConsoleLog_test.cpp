#include "brasstacks/tools/ConsoleLog.hpp"

int main() {
    btx::ConsoleLog::init();

    BTX_TRACE("Trace int '{}'!", 123);
    BTX_INFO("Info string literal '{}'!", "lolwut");

    CONSOLE_TRACE("Trace int '{}'!", 123);
    CONSOLE_INFO("Info string literal '{}'!", "lolwut");

    std::string output("something to say");
    BTX_WARN("Warn string '{}'!", output);
    CONSOLE_WARN("Warn string '{}'!", output);

    BTX_ERROR("Error float: '{:04f}'!", 3.14159f);
    CONSOLE_ERROR("Error float: '{:04f}'!", 3.14159f);

    btx::ConsoleLog::set_level(btx::ConsoleLog::Level::LOG_WARN);
    BTX_INFO("Now this won't show...");
    BTX_WARN("...but this will.");

    CONSOLE_INFO("Now this won't show...");
    CONSOLE_WARN("...but this will.");

    BTX_CRITICAL("End of the line.");
    CONSOLE_CRITICAL("Will never be reached...");

    return 0;
}