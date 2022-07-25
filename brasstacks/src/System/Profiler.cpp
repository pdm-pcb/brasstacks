#include "brasstacks/System/pch.hpp"
#include "brasstacks/System/Profiler.hpp"

#include "brasstacks/System/Clock.hpp"

#include <iomanip>

// #define BTX_PROFILE
#undef BTX_PROFILE

namespace btx {

std::mutex Profiler::_profiler_mutex;
std::string Profiler::_logfile_path;
char *Profiler::_log_data[LOG_LINES];
std::uint32_t Profiler::_log_index = 0;

void Profiler::init() {
#ifdef BTX_PROFILE
    BTX_ENGINE_TRACE("Profiler init");
    std::stringstream is;
    is << HRC::now().time_since_epoch().count();

    _logfile_path = "../../" + is.str() + "_perflog.json";

    std::unique_lock<std::mutex> lock(_profiler_mutex);
    std::fstream logfile(_logfile_path.c_str(), std::ios_base::out);

    if(!logfile.good()) {
        BTX_ENGINE_ERROR("Unable to open logfile '{}'", _logfile_path);
        assert(false);
    }

    logfile << "{\"otherData\": {},\"traceEvents\":[";
    logfile.close();

    for(std::size_t line = 0; line < sizeof(_log_data) / sizeof(char *); ++line) {
        _log_data[line] = new char[LINE_LENGTH];
        memset(_log_data[line], '\0', LINE_LENGTH);
    }
#endif // BTX_PROFILE
}

void Profiler::shutdown() {
#ifdef BTX_PROFILE
    BTX_ENGINE_TRACE("Profiler shutdown");
    if(_log_index == 0) {
        return;
    }
    std::unique_lock<std::mutex> lock(_profiler_mutex);
    std::fstream logfile(_logfile_path.c_str(), std::ios_base::out |
                                                std::ios_base::app |
                                                std::ios_base::ate);
    if(!logfile.good()) {
        BTX_ENGINE_ERROR("Unable to open logfile '{}'", _logfile_path);
        assert(false);
    }

    for(std::size_t line = 0; line < _log_index - 1; ++line) {
        logfile << _log_data[line] << ",";
    }

    logfile << _log_data[_log_index - 1] << "\n]}";
    logfile.close();

    for(std::size_t line = 0; line < sizeof(_log_data) / sizeof(char *); ++line) {
        delete[] _log_data[line];
    }
#endif // BTX_PROFILE
}

Profiler * Profiler::spawn(const char *name) {
    std::string instance_name(name);
    std::replace(instance_name.begin(), instance_name.end(), '"', '\'');
    return new Profiler(instance_name.c_str());
}

void Profiler::start() {
#ifdef BTX_PROFILE
    _start_time = HRC::now();
#endif // BTX_PROFILE
}

void Profiler::stop(const double threshold_us) {
#ifdef BTX_PROFILE
    auto dur = HRC::now() - _start_time;
    auto durcast = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
    auto ts = std::chrono::time_point_cast<std::chrono::microseconds>(_start_time).time_since_epoch().count();

    if(durcast < threshold_us) {
        return;
    }

    std::unique_lock<std::mutex> lock(_profiler_mutex);
    snprintf(
        _log_data[_log_index], LINE_LENGTH,
        "\n{ \"cat\":\"function\", "
        "\"dur\":%lli, "
        "\"name\":\"%s\", "
        "\"ph\":\"X\", "
        "\"pid\":0, "
        "\"tid\":%s, "
        "\"ts\":%lli }",
        durcast, _name.c_str(), _tid.c_str(), ts
    );
    ++_log_index;
    assert(_log_index < LOG_LINES);
#endif // BTX_PROFILE
}

Profiler::Profiler(const char *name) :
    _name       { name },
    _start_time { }
{
    std::stringstream tid;
    tid << std::this_thread::get_id();
    _tid = tid.str();
}

} // namespace btx