/**
 * calibration.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Real-time checkerboard detection for camera calibration.
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "calibration.h"
#include "checkerboard.h"

int main() {
  cv::VideoCapture cap(0);

  if (!cap.isOpened()) {
    std::cerr << "Error: could not open camera\n";
    return -1;
  }

  cv::Size patternSize(9, 6); // internal corners

  cv::Mat frame;
  std::vector<cv::Point2f> corners;

  // Save the most recent successful detection
  std::vector<cv::Point2f> last_good_corners;
  bool have_last_good = false;

  // accumulated calibration data
  std::vector<std::vector<cv::Point2f>> corner_list;
  std::vector<std::vector<cv::Vec3f>> point_list;

  while (true) {
    cap >> frame;
    if (frame.empty()) {
      std::cerr << "Error: empty frame\n";
      break;
    }

    bool found = detectCheckerboard(frame, patternSize, corners);

    if (found) {
      drawCorners(frame, patternSize, corners, found);

      last_good_corners = corners;
      have_last_good = true;

      cv::putText(frame, "Checkerboard found", cv::Point(20, 30),
                  cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);

      std::cout << "Corners detected: " << corners.size() << "\n";
      if (!corners.empty()) {
        std::cout << "First corner: (" << corners[0].x << ", " << corners[0].y
                  << ")\n";
      }
    } else {
      cv::putText(frame, "Checkerboard not found", cv::Point(20, 30),
                  cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
    }

    std::string saved_msg =
        "Saved frames: " + std::to_string(corner_list.size());
    cv::putText(frame, saved_msg, cv::Point(20, 65), cv::FONT_HERSHEY_SIMPLEX,
                0.8, cv::Scalar(255, 255, 0), 2);

    cv::imshow("Calibration", frame);

    char key = (char)cv::waitKey(1);

    if (key == 'q') {
      break;
    } else if (key == 's') {
      if (!have_last_good) {
        std::cout << "No valid checkerboard detection to save\n";
        continue;
      }

      int rc = saveCalibrationFrame(last_good_corners, patternSize, corner_list,
                                    point_list);

      if (rc == 0) {
        std::cout << "Saved calibration frame " << corner_list.size() << "\n";

        if (!point_list.empty() && !point_list.back().empty()) {
          const cv::Vec3f &p0 = point_list.back()[0];
          std::cout << "First world point: (" << p0[0] << ", " << p0[1] << ", "
                    << p0[2] << ")\n";
        }
      } else {
        std::cout << "Failed to save calibration frame\n";
      }
    }
  }

  cap.release();
  cv::destroyAllWindows();

  return 0;
}