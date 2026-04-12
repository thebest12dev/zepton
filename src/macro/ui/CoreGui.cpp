#include "CoreGui.h"
#include <fstream>
#include <filesystem>
namespace {
  std::string toFileUrl(const std::filesystem::path& p) {
    auto abs = std::filesystem::absolute(p).string();

    std::replace(abs.begin(), abs.end(), '\\', '/');
#ifdef _WIN32
    return "file:///" + abs;
#elif __linux__
    return "file://" + abs;
#endif
  }
}
namespace zepton {
  namespace ui {
    CoreGui::CoreGui() : webview(true, nullptr)
    {

    }

    void CoreGui::start()
    {
      // get html

      webview.set_title("Zepton Macro");
      webview.set_size(646, 408, WEBVIEW_HINT_FIXED);

      webview.navigate(toFileUrl("resources/zepton.html"));
      webview.run();
      
    }
  }
}


