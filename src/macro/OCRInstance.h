#pragma once
#include <string>
#include <tesseract/baseapi.h>
namespace zepton {
  struct Rect {
    float top;
    float bottom; 
    float left;
    float right;
  };

  class OCRInstance {
  private:
    Rect targetRect;
    tesseract::TessBaseAPI tesseract;
  public:
    OCRInstance();
    void setOCRCoords(Rect rect);
    void setOCRRelCoords(Rect rect);
    std::string getText();
  };
}