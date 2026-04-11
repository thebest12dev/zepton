#pragma once
#include <string>
#include "Player.h"
namespace zepton {
  // Root instance for the macro
  class Zepton {
  private:
    Zepton();
    
    Player plr;
    StatMonitor statMonitor;

  public:
    static Zepton& get();
    void startMacro();
    void pauseMacro();
    void stopMacro();
    Player& getPlayer();
    StatMonitor& getStatMonitor();
    ~Zepton();
  };
}