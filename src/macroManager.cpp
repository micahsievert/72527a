#include "macroManager.hpp"
#include "claw.hpp"

MacroManager::MacroManager(
    Pivot& pivot, 
    Lift& lift, 
    Claw& claw
)
    : pivot(pivot),
      lift(lift), 
      claw(claw) {
}

void MacroManager::start(double liftTarget, double pivotTarget, ClawState clawstate) {
    lift.setTarget(liftTarget);
    pivot.setTarget(pivotTarget);
    claw.setState(clawstate);

    running = true;
}

void MacroManager::update(){
    if (running == false) {
        return; // skip out if not running
    }

    if (lift.atTarget() && pivot.atTarget()) {
        running = false; // once lift and pivot are done, it's done running, since claw state will happen immediately.
    }
}

void MacroManager::cancel() {
    if (!running) {
        return;
    }

    // Hold at the current positions instead of continuing toward the preset.
    lift.setTarget(lift.getPosition());
    pivot.setTarget(pivot.getPosition());
    running = false;
}

bool MacroManager::isRunning() {
    return running;
}
