#include "brasstacks/log/ConsoleLog.hpp"

int main() {
    btx::ConsoleLog::set_level(btx::ConsoleLog::Level::TRACE);

    btx::ConsoleLog::trace("Trace int '{}'!", 123);

    btx::ConsoleLog::info("Info string literal '{}'!", "lolwut");

    std::string output("something to say");
    btx::ConsoleLog::warn("Warn string '{}'!", output);

    btx::ConsoleLog::error("Error float: '{:04f}'!", 3.14159f);

    btx::ConsoleLog::set_level(btx::ConsoleLog::Level::WARN);
    btx::ConsoleLog::info("Now this won't show...");
    btx::ConsoleLog::warn("...but this will.");

    btx::ConsoleLog::critical("End of the line.");

    return 0;
}