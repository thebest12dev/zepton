#pragma once
#include <string>
#include <vector>
#ifdef __linux__
//WARN: untested! evtest
#define KEY_FORWARD 17
#define KEY_BACKWARD 31
#define KEY_LEFT 30
#define KEY_RIGHT 32
#define KEY_ROTLEFT 52
#define KEY_ROTRIGHT 51
#define KEY_ROTUP 0
#define KEY_ROTDOWN 0
#define KEY_NULL 0x00
#define KEY_ESCAPE 1
#define KEY_R 19
#define KEY_ENTER 28
#define KEY_E 18
#define KEY_SPACE 57
#elif _WIN32
#define KEY_FORWARD 0x11
#define KEY_BACKWARD 0x1F
#define KEY_LEFT 0x1E
#define KEY_RIGHT 0x20
#define KEY_ROTLEFT 0x33
#define KEY_ROTRIGHT 0x34
#define KEY_ROTUP 0x00
#define KEY_ROTDOWN 0x00
#define KEY_NULL 0x00
#define KEY_ESCAPE 0x00
#define KEY_R 0x00
#define KEY_ENTER 0x00
#define KEY_E 0x12
#define KEY_SPACE 0x39
#endif

namespace zepton {
  
  struct MovementStep {
    std::vector<char> keys;
    float holdTime;
    float studs;
    
  };
  struct MovementPlan {
    std::vector<MovementStep> movementSteps;
    bool walkspeedProportionate = true;
  };
  namespace movement {
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
    const zepton::MovementPlan spawn_ramp = { .movementSteps =
      {
        {.keys = {KEY_FORWARD, KEY_RIGHT}, .holdTime = 2.95f},
        {.keys = {KEY_RIGHT}, .holdTime = 1.5f}
      }
    };
    const zepton::MovementPlan pattern_simple = { .movementSteps =
      {
        {.keys = {KEY_RIGHT}, .holdTime = 1.f},
        {.keys = {KEY_BACKWARD}, .holdTime = 0.25f},
        {.keys = {KEY_LEFT}, .holdTime = 1.f},
        {.keys = {KEY_BACKWARD}, .holdTime = 0.25f},
        {.keys = {KEY_RIGHT}, .holdTime = 1.f},
        {.keys = {KEY_RIGHT}, .holdTime = 1.f},
        {.keys = {KEY_BACKWARD}, .holdTime = 0.25f},
        {.keys = {KEY_LEFT}, .holdTime = 1.f},
        {.keys = {KEY_BACKWARD}, .holdTime = 0.25f},
        {.keys = {KEY_RIGHT}, .holdTime = 1.f},
        {.keys = {KEY_BACKWARD}, .holdTime = 0.25f},
        {.keys = {KEY_LEFT}, .holdTime = 1.f},
        {.keys = {KEY_BACKWARD}, .holdTime = 0.25f},
        {.keys = {KEY_RIGHT}, .holdTime = 1.f},
        {.keys = {KEY_RIGHT}, .holdTime = 1.f},
        {.keys = {KEY_BACKWARD}, .holdTime = 0.25f},
        {.keys = {KEY_LEFT}, .holdTime = 1.f},
        {.keys = {KEY_BACKWARD}, .holdTime = 0.25f},
        {.keys = {KEY_RIGHT}, .holdTime = 1.f},
        {.keys = {KEY_FORWARD}, .holdTime = 3.5f},
        {.keys = {KEY_RIGHT}, .holdTime = 1.5f},
      }
    };
    const zepton::MovementPlan pine_field_corner = { .movementSteps =
      {
        {.keys = {KEY_FORWARD, KEY_RIGHT}, .holdTime = 5.f}
      }
    };
    const zepton::MovementPlan pine_field_center = { .movementSteps =
      {
        {.keys = {KEY_FORWARD, KEY_RIGHT}, .holdTime = 5.f}
      }
    };
    const zepton::MovementPlan reset = { .movementSteps =
      {
        {.keys = {KEY_ESCAPE}, .holdTime = 0.25f},
        {.keys = {KEY_R}, .holdTime = 0.25f},
        {.keys = {KEY_ENTER}, .holdTime = 0.25f},
        {.keys = {KEY_NULL}, .holdTime = 6.5f} // good enough time for roblox to reset
      },
      .walkspeedProportionate = false

    };
    // to be executed from ramp

    const zepton::MovementPlan red_cannon = { .movementSteps =
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
    const zepton::MovementPlan pine_field_cannon = { .movementSteps =
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
    const zepton::MovementPlan hive6_ramp = { .movementSteps =
      {
        {.keys = {KEY_RIGHT}, .holdTime = 8.25f}
      }
    };

    const zepton::MovementPlan hive5_ramp = { .movementSteps =
      {
        {.keys = {KEY_RIGHT}, .holdTime = 6.f}
      }
    };
    const zepton::MovementPlan hive4_ramp = { .movementSteps =
      {
        {.keys = {KEY_RIGHT}, .holdTime = 3.25f}
      }
    };
    const zepton::MovementPlan hive3_ramp = { .movementSteps =
      {
        {.keys = {KEY_RIGHT}, .holdTime = 2.f}
      }
    };
    const zepton::MovementPlan hive2_ramp = { .movementSteps =
      {
        {.keys = {KEY_RIGHT}, .holdTime = 1.25f},
      }
    };
    const zepton::MovementPlan hive1_ramp = { .movementSteps =
      {
        {.keys = {KEY_RIGHT}, .holdTime = 0.5f},
      }
    };
  };
  class Player;
  class MovementManager {
  private:
    std::string location;
    Player& player;
    
  public:
    void executeMovement(MovementPlan plan);
    MovementManager(Player& player);
    void keyUp(char key, bool sc = true);
    void keyDown(char key, bool sc = true);
    void press(char key, int holdTime = 50, bool sc = true);
    void pressWalkspeedProportionate(char key, int holdTime = 50, bool sc = true);
    void moveTo(std::string targetName);
    void setLocation(std::string location);
    void walk(MovementStep step); // uses studs! NOT seconds
    void loadPathsAndPatterns();

    Player& getPlayer();
    friend class Player;
  };
}