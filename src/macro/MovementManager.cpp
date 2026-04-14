#include "MovementManager.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <sstream>

#define SSTREAM(x) (std::stringstream() << x).str()
#include <thread>
#include <chrono>
#include "Player.h"
#include <iostream>
#ifdef __linux__ 
#include <unistd.h>
#endif

#include <cstring>
#include <iostream>

#ifdef __linux__
#endif
namespace {
  using namespace zepton;
#ifdef __linux__
  std::string getDesktopEnvironment() {
    const char* xdgDesktop = std::getenv("XDG_CURRENT_DESKTOP");
    if (xdgDesktop) {
      std::string de = xdgDesktop;
      if (de.find("KDE") != std::string::npos) return "KDE";
      if (de.find("GNOME") != std::string::npos) return "GNOME";
    }

    
    const char* desktopSession = std::getenv("DESKTOP_SESSION");
    if (desktopSession) {
      std::string session = desktopSession;
      if (session.find("plasma") != std::string::npos) return "KDE";
      if (session.find("gnome") != std::string::npos) return "GNOME";
    }

    return "Unknown";
  }
#endif
  /*
    Reference walkspeed which should the patterns be executed at relative to the timings.
    Typically the developer's walkspeed, if needed change this value then change the
    patterns.

    TODO: Add a robust walk() function that does not need a reference to work with.
  */
  const int referenceWalkspeed = 29;
  /*
    Map of locations, movement spots and other paths that can be executed.

    (path) reset: Resets the character, which indirectly goes to spawn or hive.

    (location and location path) spawn: The spawn point used as a reference and to be used as an
    alternative path if needed.

    (location and location path) red_cannon: The red cannon next to the ramp which is the 
    primary route to the Pine Tree Forest (pine_field), Mountain Top Field (mountain_field) and
    Pineapple Patch (pineapple_field) and several others as secondary routes.

    (location path) spawn_ramp: Path to go from spawn to the ramp.

    (location path) hive6_ramp: Allows the 6th (leftmost) hive to go to the ramp. 

    (location path) hive5_ramp: Allows the 5th hive to go to the ramp. 

    (location path) hive4_ramp: Allows the 4th hive to go to the ramp. 

    (location path) hive3_ramp: Allows the 3th (center) hive to go to the ramp. 

    (location path) hive2_ramp: Allows the 2th hive to go to the ramp. 

    (location path) hive1_ramp: Allows the 1th (rightmost) hive to go to the ramp. 

    (location) hive: The player's hive. If hive not detected, acts as the same as spawn, and the
    location value will be changed accordingly.

    (location) hive_6: The 6th (leftmost) hive.

    (location) hive_5: The 5th hive.

    (location) hive_4: The 4th hive.

    (location) hive_3: The 3th (center) hive.

    (location) hive_2: The 2th hive.

    (location) hive_1: The 1th (rightmost) hive.

    (location) ramp: Represents the ramp next to the Ticket Shop, used primarily to access
    the 35 Bee Zone and the red cannon.
  */
  MovementPlan spawn_ramp = { .movementSteps =
    {
      {.keys = {KEY_FORWARD, KEY_RIGHT}, .holdTime = 2.95f},
      {.keys = {KEY_RIGHT}, .holdTime = 1.f}
    }
  };
  MovementPlan reset = { .movementSteps =
    {
      {.keys = {KEY_ESCAPE}, .holdTime = 0.25f},
      {.keys = {KEY_R}, .holdTime = 0.25f},
      {.keys = {KEY_ENTER}, .holdTime = 0.25f},
      {.keys = {KEY_NULL}, .holdTime = 6.5f} // good enough time for roblox to reset
    },
    .walkspeedProportionate = false
    
  };
  // to be executed from ramp

  MovementPlan red_cannon = { .movementSteps =
    {
      {.keys = {KEY_SPACE}, .holdTime = 0.5f},
      {.keys = {KEY_RIGHT, KEY_FORWARD}, .holdTime = 0.75f},
      {.keys = {KEY_SPACE}, .holdTime = 0.5f},
      {.keys = {KEY_RIGHT}, .holdTime = 0.75f},
      {.keys = {KEY_ROTRIGHT}, .holdTime = 0.1f },
      {.keys = {KEY_ROTRIGHT}, .holdTime = 0.1f},
      {.keys = {KEY_ROTRIGHT}, .holdTime = 0.1f},
      {.keys = {KEY_ROTRIGHT}, .holdTime = 0.1f},
    }
  };
  MovementPlan pine_field_cannon = { .movementSteps =
    {
      {.keys = {KEY_NULL}, .holdTime = 0.5f},
      {.keys = {KEY_SPACE}, .holdTime = 0.25f},
      {.keys = {KEY_SPACE}, .holdTime = 0.25f},
      {.keys = {KEY_LEFT, KEY_FORWARD}, .holdTime = 4.f},
      {.keys = {KEY_FORWARD}, .holdTime = 0.5f},
      {.keys = {KEY_LEFT}, .holdTime = 1.f},
      {.keys = {KEY_SPACE}, .holdTime = 0.25f},
      {.keys = {KEY_SPACE}, .holdTime = 0.25f},
      {.keys = {KEY_ROTLEFT}, .holdTime = 0.1f},
      {.keys = {KEY_ROTLEFT}, .holdTime = 0.1f},
    }
  };
  MovementPlan hive6_ramp = { .movementSteps =
    {
      {.keys = {KEY_RIGHT}, .holdTime = 8.25f}
    }
  };
  
  MovementPlan hive5_ramp = { .movementSteps =
    {
      {.keys = {KEY_RIGHT}, .holdTime = 6.f}
    }
  };
  MovementPlan hive4_ramp = { .movementSteps =
    {
      {.keys = {KEY_RIGHT}, .holdTime = 3.25f}
    }
  };
  MovementPlan hive3_ramp = { .movementSteps =
    {
      {.keys = {KEY_RIGHT}, .holdTime = 2.f}
    }
  };
  MovementPlan hive2_ramp = { .movementSteps =
    {
      {.keys = {KEY_RIGHT}, .holdTime = 1.25f},
    }
  };
  MovementPlan hive1_ramp = { .movementSteps =
    {
      {.keys = {KEY_RIGHT}, .holdTime = 0.5f},
    }
  };
}
namespace zepton {
  // Create the movement manager with our values, the player starts at spawn for now
  MovementManager::MovementManager(Player& player) : location("spawn"), player(player) {

  }
  void MovementManager::keyUp(char key, bool sc)
  {
#ifdef _WIN64
   
    keybd_event(sc ? 0 : key, sc ? key : 0, KEYEVENTF_KEYUP, 0); // use keybd_event
#elif __linux__
    // send input with x11 to sober
    std::system(SSTREAM("./ydotool key " << static_cast<int>(key) << ":0").c_str());
#endif // _WIN64
    
  }
  void MovementManager::keyDown(char key, bool sc)
  {
#ifdef _WIN64
    keybd_event(sc ? 0 : key, sc ? key : 0, 0, 0); // use keybd_event
#elif __linux__
    // send input with x11 to sober
    std::system(SSTREAM("./ydotool key " << static_cast<int>(key) << ":1").c_str());
#endif 
  }

  void MovementManager::press(char key, int holdTime, bool sc)
  {
    keyDown(key, sc);
    std::this_thread::sleep_for(std::chrono::milliseconds(holdTime));
    keyUp(key, sc);
  }

  void MovementManager::pressWalkspeedProportionate(char key, int holdTime, bool sc)
  {
    press(key, static_cast<int>((holdTime) * (static_cast<float>(referenceWalkspeed) / player.getWalkspeed())), sc);
  }
 
  // Executes a movement plan (path).
  void MovementManager::executeMovement(MovementPlan plan) {
    //focus roblox
#ifdef _WIN32
    HWND roblox = FindWindow(NULL, "Roblox");
    SetForegroundWindow(roblox);
#elif __linux__
    if (getDesktopEnvironment() == "KDE") {
      std::system(R"(./kdotool search --class "Sober" windowactivate)");
    }
    else if (getDesktopEnvironment() == "GNOME") {
      std::system(R"(xdotool search --name "Sober" windowactivate)");
    }
#endif
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    for (MovementStep step : plan.movementSteps)
    {
      float dTime = static_cast<float>((step.holdTime * 1000) * (static_cast<float>(referenceWalkspeed) /player.getWalkspeed() ));
      int time = static_cast<int>(dTime);
      for (char key : step.keys) {
        keyDown(key);
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(time));
      for (char key : step.keys) {
        keyUp(key);
      }
    }
  }
  void MovementManager::moveTo(std::string targetName) {
    Logger::info("Moving to " + targetName + "...", "Movement");
#define NOT_IN_LOCATION(target) (targetName == target && location != target)
    if (NOT_IN_LOCATION("spawn_ramp")) {
      if (location != "spawn") {
        moveTo("spawn");
      }
      executeMovement(spawn_ramp);
      location = "ramp";
    }
    else if (NOT_IN_LOCATION("hive_ramp")) {
      if (location == "hive") {

        // optimized for the specific hive
        switch (player.getHive()) {
        case 6: {
          // hive6_ramp
          executeMovement(hive6_ramp);
          break;
        }
        case 5: {
          // hive5_ramp
          executeMovement(hive5_ramp);
          break;
        }
        case 4: {
          // hive4_ramp
          executeMovement(hive4_ramp);
          break;
        }
        case 3: {
          // hive3_ramp
          executeMovement(hive3_ramp);
          break;
        }
        case 2: {
          // hive2_ramp
          executeMovement(hive2_ramp);
          break;
        }
        case 1: {
          // hive1_ramp
          executeMovement(hive1_ramp);
          break;
        }

        }
        location = "ramp";
      }
    } else if (NOT_IN_LOCATION("hive_reset")) {
      if (location != "hive") {
        executeMovement(reset);
        location = "hive";
      }
      
    }
    else if (NOT_IN_LOCATION("red_cannon")) {
      if (location == "ramp") {
        executeMovement(red_cannon);
      }
      else {
        moveTo("ramp");
        executeMovement(red_cannon);
      }
      
    }
    else if (NOT_IN_LOCATION("ramp")) {
      if (location == "spawn") {
        moveTo("spawn_ramp");
      }
      else if (location == "hive") {
        moveTo("hive_ramp");
      }
    }
    else if (NOT_IN_LOCATION("pine_field_cannon")) {
      if (location != "pine_field") {
        executeMovement(pine_field_cannon);
      }
    }
  }
  void MovementManager::setLocation(std::string location)
  {
    this->location = location;
  }
  Player& MovementManager::getPlayer()
  {
    return player;
  }
}
