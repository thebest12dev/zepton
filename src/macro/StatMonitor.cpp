#include "StatMonitor.h"
#include <opencv2/opencv.hpp>
#include <utility>
#include <memory>
namespace zepton {
    
    void StatMonitor::backgroundWorker() {
        /*
        POSTPONED: The haste detection has been quite difficult recently due to different codebases (OpenCV 
        and Gdip_ImageSearch), and due to some technical debt, work on it has paused until no earlier than 0.4.0.
        */
        return;
    }
    void StatMonitor::startThread() {
        backgroundThread = std::thread(&StatMonitor::backgroundWorker, this);
        backgroundThread.join();

    }
}