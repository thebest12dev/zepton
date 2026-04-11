#include "Zepton.h"
#include "../Logger.h"
#include <iostream>
namespace zepton {
  Zepton::Zepton()
  {
  }

  Zepton& Zepton::get()
  {
    static Zepton zepton;
    return zepton;
  }

  
  void Zepton::startMacro() {
    
  }
  Zepton::~Zepton() {

  }
}

