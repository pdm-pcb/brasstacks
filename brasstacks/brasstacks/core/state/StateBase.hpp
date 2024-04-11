#ifndef BRASSTACKS_CORE_STATE_STATEBASE_HPP
#define BRASSTACKS_CORE_STATE_STATEBASE_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/state_events.hpp"

namespace btx {

class StateBase {
public:
    virtual ~StateBase() = default;

    virtual void enter()   = 0;
    virtual void exit()    = 0;
    virtual void execute() = 0;

protected:
    StateBase() = default;
};

} // namespace btx

#endif // BRASSTACKS_CORE_STATE_STATEBASE_HPP