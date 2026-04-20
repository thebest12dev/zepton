#include "OCRInstance.h"
#ifdef _WIN32
#include <Windows.h>
#endif
#include <opencv4/opencv2/opencv.hpp>
#ifdef __linux__
#include <unistd.h>
#include <sys/wait.h>
#endif
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
namespace {
#ifdef __linux__
  std::string getDesktopEnvironment() {
	const char* sessionType = std::getenv("XDG_SESSION_TYPE");

    if (sessionType != nullptr) {
        return sessionType;
    } else {
        
        if (std::getenv("WAYLAND_DISPLAY")) {
            return "wayland";
        } else if (std::getenv("DISPLAY")) {
            return "x11";
        } 
    }
return "unknown";
}
  std::vector<unsigned char> linuxCaptureScreen() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
      throw std::runtime_error("pipe failed");
    }

    pid_t pid = fork();
    if (pid == -1) {
      throw std::runtime_error("fork failed");
    }

    if (pid == 0) {
      close(pipefd[0]);

      dup2(pipefd[1], STDOUT_FILENO);
      close(pipefd[1]);
      if (getDesktopEnvironment() == "wayland") {
	//assuming kde
      char* args[] = {
          (char*)"spectacle",
          (char*)"-bfo",
          (char*)"/dev/stdout",
          nullptr
};
        execvp("spectacle", args);
      } else if (getDesktopEnvironment() == "x11") {
	  char* args[] = {
          (char*)"scrot",
          (char*)"-q",
          (char*)"100",
	  (char*)"--silent",
	  (char*)"-",
          nullptr};
	  execvp("scrot", args);
      };
_exit(1);
   }
    

    close(pipefd[1]); 

    std::vector<unsigned char> buffer;
    char temp[4096];

    ssize_t n;
    while ((n = read(pipefd[0], temp, sizeof(temp))) > 0) {
      buffer.insert(buffer.end(), temp, temp + n);
    }

    close(pipefd[0]);
    waitpid(pid, nullptr, 0);

    return buffer;
  }
  cv::Mat linuxPngToCVMat(const std::vector<unsigned char>& pngBytes) {
    cv::Mat img = cv::imdecode(pngBytes, cv::IMREAD_UNCHANGED);

    if (img.empty()) {
      throw std::runtime_error("failed to decode PNG to cv::Mat!");
    }

    return img;
  }
#elif _WIN32

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
#endif
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
  
  cv::Rect OCRInstance::toAbsoluteRect(zepton::Rect rect, cv::Mat& mat)
  {
      int x1 = static_cast<int>(rect.left * mat.cols);
      int y1 = static_cast<int>(rect.top * mat.rows);
      int x2 = static_cast<int>(rect.right * mat.cols);
      int y2 = static_cast<int>(rect.bottom * mat.rows);

      return cv::Rect(x1, y1, x2 - x1, y2 - y1);
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
#ifdef _WIN32
    auto bitmap = winCaptureScreen();
    cv::Mat mat = winHBitmapToCVMat(bitmap);
#elif __linux__
    auto bitmap = linuxCaptureScreen();
    cv::Mat mat = linuxPngToCVMat(bitmap);
#endif
    cv::Mat cropped = matRelCoords(mat, targetRect);
    cv::Mat thresh = prepareForTesseract(cropped);

    tesseract.SetImage(thresh.data, thresh.cols, thresh.rows, 1, thresh.step);

    char* out = tesseract.GetUTF8Text();
    std::string output(out);

    delete[] out;
    return output;
  }
  cv::Mat OCRInstance::getScreen() {
#ifdef _WIN32
      auto bitmap = winCaptureScreen();
      cv::Mat mat = winHBitmapToCVMat(bitmap);
#elif __linux__
      auto bitmap = linuxCaptureScreen();
      cv::Mat mat = linuxPngToCVMat(bitmap);
#endif
      return mat;
  }
}
