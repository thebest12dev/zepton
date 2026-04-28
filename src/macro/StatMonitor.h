#pragma once
#include "OCRInstance.h"
#include <functional>
#include <thread>
namespace zepton {
  class StatMonitor {
  private:
	  OCRInstance ocr;
	  std::function<void(int)> walkspeedUpdate;
	  int walkspeed;
	  void backgroundWorker();
	  std::thread backgroundThread;
  public:
	  // Creates an independent thread for OCR.
	  StatMonitor();
	  void startThread();
	  void stopThread();
	  void setWalkspeed(int walkspeed);
	  void onWalkspeedUpdate(std::function<void(int updatedWalkspeed)> callback);
	  ~StatMonitor();
  };
}