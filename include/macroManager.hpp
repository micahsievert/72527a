#pragma once

#include "pivot.hpp"
#include "lift.hpp"
#include "claw.hpp"

#include <cstdint>


class MacroManager {
    public:
        MacroManager(Pivot& pivot, Lift& lift, Claw& claw);

        void start(double liftTarget, double pivotTarget, ClawState clawstate);

        void update();
        void cancel();
        bool isRunning();


    private:
        Pivot& pivot;
        Lift& lift;
        Claw& claw;

        bool running = false;
};
