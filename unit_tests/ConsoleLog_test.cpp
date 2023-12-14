#include "brasstacks/tools/ConsoleLog.hpp"

int main() {
    btx::ConsoleLog::init();

    BTX_TRACE("Trace int '{}'!", 123);
    BTX_INFO("Info string literal '{}'!", "lolwut");

    BTX_TRACE("Trace int '{}'!", 123);
    BTX_INFO("Info string literal '{}'!", "lolwut");

    std::string output("something to say");
    BTX_WARN("Warn string '{}'!", output);
    BTX_WARN("Warn string '{}'!", output);

    BTX_ERROR("Error float: '{:04f}'!", 3.14159f);
    BTX_ERROR("Error float: '{:04f}'!", 3.14159f);

    btx::ConsoleLog::set_level(btx::ConsoleLog::Level::LOG_WARN);
    BTX_INFO("Now this won't show...");
    BTX_WARN("...but this will.");

    BTX_INFO("Now this won't show...");
    BTX_WARN("...but this will.");

    BTX_CRITICAL("End of the line.");
    BTX_CRITICAL("Will never be reached...");

    return 0;
}