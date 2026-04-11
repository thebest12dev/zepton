#include "OCRInstance.h"
#include <Windows.h>
#include <opencv4/opencv2/opencv.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
namespace {

  HBITMAP winCaptureScreen() {
    HDC hScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hScreen);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, screenWidth, screenHeight);
    SelectObject(hdcMem, hBitmap);

    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hScreen, 0, 0, SRCCOPY);
    return hBitmap;
  }
  cv::Mat winHBitmapToCVMat(HBITMAP hBitmap) {
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    cv::Mat mat(bmp.bmHeight, bmp.bmWidth, CV_8UC4);
    GetBitmapBits(hBitmap, bmp.bmHeight * bmp.bmWidth * 4, mat.data);
    return mat;
  }
  cv::Mat matRelCoords(cv::Mat& mat, zepton::Rect& rect) {
    float x1 = rect.left;
    float x2 = rect.right;
    float y1 = rect.top;
    float y2 = rect.bottom;

    int px1 = static_cast<int>(x1 * mat.cols);
    int py1 = static_cast<int>(y1 * mat.rows);
    int px2 = static_cast<int>(x2 * mat.cols);
    int py2 = static_cast<int>(y2 * mat.rows);

    cv::Rect roi(px1, py1, px2 - px1, py2 - py1);
    cv::Mat cropped = mat(roi);
    return cropped;
  }
  cv::Mat prepareForTesseract(cv::Mat& mat) {
    cv::Mat gray, thresh;

    cv::cvtColor(mat, gray, cv::COLOR_BGRA2GRAY);
    cv::threshold(gray, thresh, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    return thresh;
  }
}
namespace zepton {
  OCRInstance::OCRInstance() : targetRect({ 0,0,0,0 }), tesseract()
  {
    tesseract.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

    tesseract.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
  }
  void OCRInstance::setOCRCoords(Rect rect)
  {
    targetRect = rect;
  }
  void OCRInstance::setOCRRelCoords(Rect rect)
  {
    targetRect = rect;
  }
  std::string OCRInstance::getText()
  {
    auto bitmap = winCaptureScreen();
    cv::Mat mat = winHBitmapToCVMat(bitmap);
    cv::Mat cropped = matRelCoords(mat, targetRect);
    cv::Mat thresh = prepareForTesseract(cropped);

    tesseract.SetImage(thresh.data, thresh.cols, thresh.rows, 1, thresh.step);

    char* out = tesseract.GetUTF8Text();
    std::string output(out);

    delete[] out;
    return output;
  }
}