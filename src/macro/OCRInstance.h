#pragma once
#include <string>
#include <tesseract/baseapi.h>
#include <opencv2/opencv.hpp>
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
    cv::Mat getScreen();
    cv::Rect toAbsoluteRect(zepton::Rect rect, cv::Mat& mat);
    std::string getText();
  };
}