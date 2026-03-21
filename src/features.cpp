/**
 * features.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Live ORB feature detection demo.
 */

#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>

#include "features.h"
#include "image_io.h"

int main() {
  cv::VideoCapture cap(0);

  if (!cap.isOpened()) {
    std::cerr << "Error: could not open camera\n";
    return -1;
  }

  int maxFeatures = 300;
  int fastThreshold = 20;

  cv::Mat frame, display;
  std::vector<cv::KeyPoint> keypoints;
  cv::Mat descriptors;

  while (true) {
    cap >> frame;
    if (frame.empty()) {
      std::cerr << "Error: empty frame\n";
      break;
    }

    display = frame.clone();

    int rc = detectORBFeatures(frame, maxFeatures, fastThreshold, keypoints,
                               descriptors);

    if (rc == 0) {
      drawFeaturePoints(display, keypoints);
    }

    std::string kp_msg = "Keypoints: " + std::to_string(keypoints.size());
    std::string feat_msg = "Max features: " + std::to_string(maxFeatures);
    std::string thr_msg = "FAST threshold: " + std::to_string(fastThreshold);

    cv::putText(display, kp_msg, cv::Point(20, 30), cv::FONT_HERSHEY_SIMPLEX,
                0.7, cv::Scalar(0, 255, 255), 2);

    cv::putText(display, feat_msg, cv::Point(20, 60), cv::FONT_HERSHEY_SIMPLEX,
                0.7, cv::Scalar(255, 255, 0), 2);

    cv::putText(display, thr_msg, cv::Point(20, 90), cv::FONT_HERSHEY_SIMPLEX,
                0.7, cv::Scalar(255, 0, 255), 2);

    cv::putText(display,
                "[=/-] max features  [t/g] threshold  [p] screenshot  [q] quit",
                cv::Point(20, display.rows - 20), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                cv::Scalar(255, 255, 255), 1);

    cv::imshow("ORB Features", display);

    char key = (char)cv::waitKey(1);

    if (key == 'q') {
      break;
    } else if (key == '=') {
      maxFeatures += 50;
    } else if (key == '-') {
      maxFeatures = std::max(50, maxFeatures - 50);
    } else if (key == 't') {
      fastThreshold += 2;
    } else if (key == 'g') {
      fastThreshold = std::max(1, fastThreshold - 2);
    } else if (key == 'p') {
      int save_rc = saveImage("out/images", "features", display);
      if (save_rc != 0) {
        std::cout << "Failed to save screenshot\n";
      }
    }
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}