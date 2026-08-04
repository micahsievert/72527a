#include "macroManager.hpp"

MacroManager::MacroManager(
    Arm& arm, 
    Lift& lift, 
    Claw& claw
)
    : arm(arm), 
      lift(lift), 
      claw(claw) {
}

void MacroManager::update() { // central updater which will call the proper macro updater
    switch (activeMacro) {
        case MacroType::MatchLoad:
            updateMatchLoad();
            break;
        case MacroType::LowScore:
            updateLowScore();
            break;
        default:
            break;
    }
}

void MacroManager::cancel() {
    activeMacro = MacroType::None;
    currentMatchLoadStep = MatchLoadStep::Idle;
    currentLowScoreStep = LowScoreStep::Idle;
}

constexpr double safeLiftHeightForArmMovement = 100.0;
constexpr double freeArmThreshold = 50.0;
constexpr double matchLoadingArmPosition = 150.0;
constexpr double lowScoreLiftHeight = 300.0;
constexpr double lowScoreArmPosition = 125.0;

void MacroManager::startMatchLoad() {
    activeMacro = MacroType::MatchLoad;
    currentMatchLoadStep = MatchLoadStep::RaiseLift;
}
void MacroManager::startLowScore() {
    activeMacro = MacroType::LowScore;
    currentLowScoreStep = LowScoreStep::RaiseLift;
}

void MacroManager::updateMatchLoad() {
    switch (currentMatchLoadStep) {
        case MatchLoadStep::Idle:
            break;
        
        case MatchLoadStep::RaiseLift:
            lift.setTarget(safeLiftHeightForArmMovement);
            if (lift.atTarget(8.0)) {
                currentMatchLoadStep = MatchLoadStep::FreeArm;
            }
            break;
    
        case MatchLoadStep::FreeArm:
            arm.setArmTarget(freeArmThreshold);
            if (arm.getArmAngle() > freeArmThreshold - 5.0) { // only make sure its enough out of the intake, otherwise movement is skipped
                currentMatchLoadStep = MatchLoadStep::WristToMatchLoad;
            }
            break;
    
        case MatchLoadStep::WristToMatchLoad:
            arm.setWristLevelPosition();
            currentMatchLoadStep = MatchLoadStep::FinishArmMove;
            break;
    
        case MatchLoadStep::FinishArmMove:
            arm.setArmTarget(matchLoadingArmPosition);
            if (arm.armAtTarget(3.0)) {
                currentMatchLoadStep = MatchLoadStep::OpenClaw;
            }
            break;
    
        case MatchLoadStep::OpenClaw:
            claw.setState(ClawState::Open);
            currentMatchLoadStep = MatchLoadStep::Complete;
            break;
    
        case MatchLoadStep::Complete:
            activeMacro = MacroType::None;
            currentMatchLoadStep = MatchLoadStep::Idle;
            break;
        
    }
}

void MacroManager::updateLowScore() {
    switch (currentLowScoreStep) {
        case LowScoreStep::Idle:
            break;
        
        case LowScoreStep::RaiseLift:
            lift.setTarget(lowScoreLiftHeight);
            if (lift.atTarget(8.0)) {
                currentLowScoreStep = LowScoreStep::FreeArm;
            }
            break;

        case LowScoreStep::FreeArm:
            arm.setArmTarget(freeArmThreshold);
            if (arm.getArmAngle() > freeArmThreshold - 5.0) { // only make sure its enough out of the intake, otherwise movement is skipped
                currentLowScoreStep = LowScoreStep::LevelWrist;
            }
            break;

        case LowScoreStep::LevelWrist:
            arm.setWristLevelPosition();
            currentLowScoreStep = LowScoreStep::PositionArm;
            break;

        case LowScoreStep::PositionArm:
            arm.setArmTarget(lowScoreArmPosition);
            if (arm.armAtTarget(3.0)) {
                currentLowScoreStep = LowScoreStep::Complete;
            }
            break;
    
        case LowScoreStep::Complete:
            activeMacro = MacroType::None;
            currentLowScoreStep = LowScoreStep::Idle;
            break;
        
    }
}
