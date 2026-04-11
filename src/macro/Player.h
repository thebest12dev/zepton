#pragma once
#include <string>
#include "StatMonitor.h"
#include "MovementManager.h"
#include "OCRInstance.h"
#include "../Logger.h"
namespace zepton {
  class Player {
    MovementManager movementManager;
    OCRInstance ocr;
    int walkspeed;
    int hiveNum;
  public:
    Player();
    int getWalkspeed();
    void setWalkspeed(int walkspeed);
    void autoDetectWalkspeed();
    std::string getCurrentLocation();
    void attachStatMonitor(StatMonitor monitor);
    uint64_t getHoneyPerSecond();
    uint64_t getCurrentHoney();
    void getPing();
    int getHive();
    void claimHive();
    void resetCharacter();
    void convertHoney();
    void goToField(std::string fieldId);
    MovementManager& getMovementManager();
  };
}