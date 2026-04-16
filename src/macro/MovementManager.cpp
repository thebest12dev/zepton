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
#include <filesystem>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#ifdef __linux__
#endif
#define CONST_BACKING_KEY "__const_backing"
#define CONST_MT_INSTALLED "__const_mt_installed"
namespace {

  using namespace zepton;
  MovementPlan movementPlan = {};
  int luaWalk(lua_State* L) {
      std::vector<char> movementKeys;
      int n = lua_gettop(L);
      if (n < 2) {
          return luaL_error(L, "too few arguments");
      }
      else if (n > 2) {
          return luaL_error(L, "too many arguments");
      }
      else if (n == 2) {
          if (!lua_istable(L, 1)) {
              return luaL_error(L, "expected an array of keycodes, got %s", lua_typename(L, lua_type(L, 1)));
          }

          size_t len = lua_objlen(L, 1);

          for (int i = 1; i <= len; i++) {
              lua_pushinteger(L, i);
              lua_gettable(L, 1);
              int val = luaL_checkinteger(L, -1);
              movementKeys.push_back(val);

              lua_pop(L, 1);
          }
          float val = static_cast<float>(luaL_checknumber(L, 2));

          movementPlan.movementSteps.push_back(MovementStep{ .keys = movementKeys, .studs = val });
      }
      return 0;
  };
  int luaPressKeys(lua_State* L) {
      std::vector<char> movementKeys;
      int n = lua_gettop(L);
      if (n < 2) {
          return luaL_error(L, "too few arguments");
      }
      else if (n > 2) {
          return luaL_error(L, "too many arguments");
      }
      else if (n == 2) {
          if (!lua_istable(L, 1)) {
              return luaL_error(L, "expected an array of keycodes, got %s", lua_typename(L, lua_type(L, 1)));
          }

          size_t len = lua_objlen(L, 1);

          for (int i = 1; i <= len; i++) {
              lua_pushinteger(L, i);
              lua_gettable(L, 1);
              int val = luaL_checkinteger(L, -1);
              movementKeys.push_back(val);

              lua_pop(L, 1);
          }
          float val = static_cast<float>(luaL_checknumber(L, 2));

          movementPlan.movementSteps.push_back(MovementStep{ .keys = movementKeys, .holdTime = val, .walkspeedProportionate = false });
      }
      return 0;
  }
  static void setReadonlyGlobal(lua_State* L, int envIndex, const char* key)
  {
      lua_pushvalue(L, -1);              
      lua_setfield(L, envIndex, key);   
      lua_pop(L, 1);
  }
  void createSandbox(lua_State* L) {
    lua_newtable(L);
    int env = lua_gettop(L);
    // always need this
    lua_getglobal(L, "math");
    lua_setfield(L, -2, "math");

    lua_getglobal(L, "string");
    lua_setfield(L, -2, "string");

    lua_getglobal(L, "table");
    lua_setfield(L, -2, "table");

    lua_getglobal(L, "pairs");
    lua_setfield(L, -2, "pairs");

    lua_getglobal(L, "ipairs");
    lua_setfield(L, -2, "ipairs");

    lua_getglobal(L, "tostring");
    lua_setfield(L, -2, "tostring");

    lua_getglobal(L, "tonumber");
    lua_setfield(L, -2, "tonumber");
    
    //zepton globals
    lua_pushinteger(L, KEY_FORWARD);
    setReadonlyGlobal(L, env, "KEY_FORWARD");
    lua_pushinteger(L,KEY_LEFT);
    setReadonlyGlobal(L, env,"KEY_LEFT");
    lua_pushinteger(L,KEY_RIGHT);
    setReadonlyGlobal(L, env,"KEY_RIGHT");
    lua_pushinteger(L,KEY_SPACE);
    setReadonlyGlobal(L, env,"KEY_SPACE");
    lua_pushinteger(L,KEY_ROTLEFT);
    setReadonlyGlobal(L, env,"KEY_ROTLEFT");
    lua_pushinteger(L,KEY_ROTRIGHT);
    setReadonlyGlobal(L, env,"KEY_ROTRIGHT");
    
    // walk(keys_to_press, studs_to_walk)
    lua_pushcfunction(L, luaWalk);

    lua_setfield(L, -2, "walk");
    lua_pushcfunction(L, luaPressKeys);

    lua_setfield(L, -2, "press_keys");

    lua_newtable(L);
    lua_pushliteral(L, "locked");
    lua_setfield(L, -2, "__metatable");
    lua_setmetatable(L, -2);
  };
  
  zepton::MovementPlan evaluateScript(lua_State* L, const char* filename) {
    movementPlan = MovementPlan{};

    if (luaL_loadfile(L, filename) != 0) {
      printf("load error: %s\n", lua_tostring(L, -1));
      lua_pop(L, 1);
      return {};
    }

    createSandbox(L);

    lua_pushvalue(L, -1);
    lua_setfenv(L, -3);
    lua_pop(L, 1);

    if (lua_pcall(L, 0, 0, 0) != 0) {
      printf("runtime error: %s\n", lua_tostring(L, -1));
      lua_pop(L, 1);
      return {};
    }

    return movementPlan;
  };
  
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
 
}
using namespace zepton::movement;
#include <fstream>
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
 
  void MovementManager::walk(MovementStep step) {
    
    float dTime = static_cast<float>((step.studs) / player.getWalkspeed());
    int time = static_cast<int>(dTime * 1000);
    for (char key : step.keys) {
      keyDown(key);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
    for (char key : step.keys) {
      keyUp(key);
    }
  }
  
  void MovementManager::loadPathsAndPatterns() {
    lua_State* L = luaL_newstate();
    try {
        if (std::filesystem::exists("paths") && std::filesystem::is_directory("paths")) {
        for (const auto& entry : std::filesystem::directory_iterator("paths")) {
          if (entry.is_regular_file()) {
              std::string file = entry.path().string();
              std::string fileName = entry.path().stem().string();
              MovementPlan evaluatedPlan = evaluateScript(L, file.c_str());
              pathRegistry[fileName] = evaluatedPlan;
          }
        }
      }
    }
    catch (const std::filesystem::filesystem_error& e) {
      std::cerr << "error: " << e.what() << std::endl;
    }
    // todo: add to registry
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
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    for (MovementStep step : plan.movementSteps)
    {
      if (step.studs > 0) {
          float dTime = static_cast<float>((step.studs) / player.getWalkspeed());
          int time = static_cast<int>(dTime * 1000);
          for (char key : step.keys) {
              keyDown(key);
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(time));
          for (char key : step.keys) {
              keyUp(key);
          }
      }
      else if (step.holdTime > 0) {
          int time = static_cast<int>(step.holdTime * 1000);
          if (step.walkspeedProportionate) {
              float dTime = static_cast<float>((step.holdTime * 1000) * (static_cast<float>(referenceWalkspeed) / player.getWalkspeed()));
              time = static_cast<int>(dTime);
          }
          
          
          for (char key : step.keys) {
              keyDown(key);
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(time));
          for (char key : step.keys) {
              keyUp(key);
          }
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
      if (pathRegistry.contains("spawn_ramp")) {
          executeMovement(pathRegistry["spawn_ramp"]);
          location = "ramp";
      }
      else {

      }
      
      
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
          if (pathRegistry.contains("red_cannon")) {
              executeMovement(pathRegistry["red_cannon"]);
              location = "red_cannon";
          }
          else {

          }
      }
      else {
        moveTo("ramp");
        if (pathRegistry.contains("red_cannon")) {
            executeMovement(pathRegistry["red_cannon"]);
            location = "red_cannon";
        }
        else {

        }
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
