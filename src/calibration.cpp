/**
 * calibration.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Real-time checkerboard detection for camera calibration.
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "checkerboard.h"

int main() {
  cv::VideoCapture cap(0);

  if (!cap.isOpened()) {
    std::cerr << "Error: could not open camera\n";
    return -1;
  }

  cv::Size patternSize(9, 6);
  std::vector<cv::Point2f> corners;

  cv::Mat frame;

  while (true) {
    cap >> frame;
    if (frame.empty())
      break;

    bool found = detectCheckerboard(frame, patternSize, corners);

    if (found) {
      drawCorners(frame, patternSize, corners, found);

      std::cout << "Corners: " << corners.size() << "\n";

      if (!corners.empty()) {
        std::cout << "First corner: (" << corners[0].x << ", " << corners[0].y
                  << ")\n";
      }
    }

    cv::imshow("Checkerboard", frame);

    char key = cv::waitKey(1);
    if (key == 'q')
      break;
  }

  return 0;
}