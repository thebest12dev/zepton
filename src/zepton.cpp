// zepton0.cpp : Defines the entry point for the application.
//

#include "macro/Zepton.h"
#include "Logger.h"
#include "macro/Player.h"
#include "macro/MovementManager.h"
#include "macro/OCRInstance.h"
#include "macro/ui/CoreGui.h"
int main()
{
  std::cout << "Zepton Macro 0.2.0" << std::endl;
  zepton::Logger::info("Starting macro...", "Zepton");
  zepton::Logger::info("OCR identifier: tesseract (eng)", "Zepton");
  zepton::ui::CoreGui gui;
  gui.start();
  //zepton::Player player;
 // player.claimHive();
 // player.convertHoney();
  //player.getMovementManager().moveTo("spawn_ramp");
  //player.claimHive();
  //player.convertHoney();
  //player.goToField("pine_field");
  //auto zepton = zepton::Zepton::get();
  //zepton.start();
  return 0;
}

