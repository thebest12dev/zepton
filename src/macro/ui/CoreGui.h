#pragma once
#include <webview/webview.h>
namespace zepton {
  namespace ui {
    class CoreGui {
    private:
      webview::webview webview;
    public:
      CoreGui();
      void start();
    };
  }
}