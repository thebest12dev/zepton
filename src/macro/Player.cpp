#include "Player.h"
#include <sstream>
#include <chrono>
#include <thread>
#include "StatMonitor.h"
#ifdef _WIN32
#include <Windows.h>
#endif
#define SSTREAM(x) (std::stringstream() << x).str()
namespace {
  zepton::Rect dialog = { 0.075f, 0.160f, 0.36f, 0.628f };
}
namespace zepton {
  Player::Player() : movementManager(*this), ocr(), walkspeed(29), hiveNum(5)
  {
    
  }
  int Player::getWalkspeed()
  {
    return walkspeed;
  }
  void Player::setWalkspeed(int walkspeed)
  {
    walkspeed = walkspeed;
  }
  void Player::autoDetectWalkspeed()
  {
  }
  std::string Player::getCurrentLocation()
  {
    return movementManager.location;
  }
  void Player::attachStatMonitor(StatMonitor* monitor)
  {
      statMonitor = monitor;
      statMonitor->setWalkspeed(walkspeed);
      statMonitor->onWalkspeedUpdate([this](int newWalkspeed) {
          walkspeed = newWalkspeed;
          });
      statMonitor->startThread();
  }
  uint64_t Player::getHoneyPerSecond()
  {
    return 0;
  }
  uint64_t Player::getCurrentHoney()
  {
    return 0;
  }
  void Player::getPing()
  {
  }
  int Player::getHive()
  {
    return hiveNum;
  }
  void Player::claimHive() {
    Logger::info("Attempting to claim hive...", "Hive");
    movementManager.moveTo("spawn_ramp");
    movementManager.pressWalkspeedProportionate(KEY_LEFT, 500);
    ocr.setOCRRelCoords(dialog);
    
    int hive = 1;
    bool hiveClaimed = false;
    auto attemptClaimHive = [this, &hive, &hiveClaimed]() {
      if (!hiveClaimed) {
        std::string text = ocr.getText();
        if (text.find("Claim") != std::string::npos || text.find("Hive") != std::string::npos) {
          movementManager.press(KEY_E);
          Logger::info(SSTREAM("Claimed hive " << hive << "!"), "Hive");
          hiveClaimed = true;

          return;
        }
        else {
          Logger::info(SSTREAM("Hive not found! Trying again... (" << hive << "/6)"), "Hive");
          movementManager.pressWalkspeedProportionate(KEY_LEFT, 1300);
          hive++;
          return;
        }
      }
      
    };
    for (int i = 0; i < 6; i++)
    {
      attemptClaimHive();
    }
    if (hiveClaimed == false) {
      Logger::info("Hive not found!", "Hive");
    }
    else {
      movementManager.location = "hive";
    }
  }
  void Player::resetCharacter()
  {
    movementManager.moveTo("reset");
  }
  void Player::convertHoney()
  {
    zepton::Logger::info("Going to hive to convert!", "Movement");
    bool reset = true;
    if (reset) {
      movementManager.moveTo("hive_reset");
      movementManager.press(KEY_E);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      while (true) {
        ocr.setOCRRelCoords(dialog);
        std::string text = ocr.getText();
        if (text.find("Stop") != std::string::npos || text.find("Making Honey") != std::string::npos) {
          continue;
        }
        else if (text.find("Flower") != std::string::npos || text.find("Pollen") != std::string::npos) {
          zepton::Logger::info("Finished converting!", "Hive");
          break;
        }
        else if (text.find("Make Honey") != std::string::npos) {
          zepton::Logger::info("Make Honey prompt detected while converting!", "Hive");
          movementManager.press(KEY_E);
          std::this_thread::sleep_for(std::chrono::milliseconds(500));
          continue;
        } 
      }
      return;
    }
  }
  void Player::goToField(std::string fieldId)
  {
    
    if (fieldId == "pine_field" && movementManager.location != "pine_field") {
      Logger::info("Going to the Pine Tree Forest...", "Movement");
      movementManager.moveTo("red_cannon");
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      ocr.setOCRRelCoords(dialog);
      std::string text = ocr.getText();
      if (text.find("Red Cannon") != std::string::npos) {
        movementManager.press(KEY_E);
        movementManager.moveTo("pine_field_cannon");
      }
    }
  }
  MovementManager& Player::getMovementManager()
  {
    return movementManager;
  }
}
