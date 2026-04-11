#pragma once
#include <string>
#include <iostream>
namespace zepton {
  class Logger {
  public:
    static void info(std::string log, std::string context = "unassigned") {
      std::cout << "\033[90m("<< context <<") \033[0m\033[92mINFO: \033[0m" << "\033[90m"<< log <<"\033[0m" << std::endl;
    }
  };
}