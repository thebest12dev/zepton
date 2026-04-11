#pragma once
#include <string>
#include <vector>
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
namespace zepton {
  struct MovementStep {
    std::vector<char> keys;
    float holdTime;
  };
  struct MovementPlan {
    std::vector<MovementStep> movementSteps;
    bool walkspeedProportionate = true;
  };
  class Player;
  class MovementManager {
  private:
    std::string location;
    Player& player;
    void executeMovement(MovementPlan plan);
  public:
    MovementManager(Player& player);
    void keyUp(char key, bool sc = true);
    void keyDown(char key, bool sc = true);
    void press(char key, int holdTime = 50, bool sc = true);
    void pressWalkspeedProportionate(char key, int holdTime = 50, bool sc = true);
    void moveTo(std::string targetName);
    void setLocation(std::string location);
    Player& getPlayer();
    friend class Player;
  };
}