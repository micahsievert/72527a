#pragma once

#include "arm.hpp"
#include "lift.hpp"
#include "claw.hpp"

enum class MacroType {
    None,
    Intake,
    MatchLoad,
    LowScore,
    HighScore
};

enum class MatchLoadStep {
    Idle,
    RaiseLift,
    FreeArm, // Raise arm past a safe threshold before moving wrist
    WristToMatchLoad,
    FinishArmMove,
    OpenClaw,
    Complete
};
enum class LowScoreStep {
    Idle,
    RaiseLift,
    FreeArm,
    LevelWrist,
    PositionArm,
    Complete
};

class MacroManager {
    public:
        MacroManager(Arm& arm, Lift& lift, Claw& claw);

        void startMatchLoad();
        void startLowScore();

        void update();
        void cancel();


    private:
        Arm& arm;
        Lift& lift;
        Claw& claw;

        MacroType activeMacro = MacroType::None;

        void updateMatchLoad();
        MatchLoadStep currentMatchLoadStep = MatchLoadStep::Idle;
        
        void updateLowScore();
        LowScoreStep currentLowScoreStep = LowScoreStep::Idle;
};