#pragma once
#include <string>
#include <cstdint>
#include "StatMonitor.h"
#include "MovementManager.h"
#include "OCRInstance.h"
#include "../Logger.h"
#include <optional>
namespace zepton {
  class Player {
  private:
    MovementManager movementManager;
    OCRInstance ocr;
    int walkspeed;
    int hiveNum;
    StatMonitor* statMonitor = nullptr;
  public:
    Player();
    int getWalkspeed();
    void setWalkspeed(int walkspeed);
    void autoDetectWalkspeed();
    std::string getCurrentLocation();
    void attachStatMonitor(StatMonitor* monitor);
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
