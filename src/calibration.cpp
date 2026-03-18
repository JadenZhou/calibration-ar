/**
 * calibration.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Real-time checkerboard detection for camera calibration.
 */

#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>

#include "calibration.h"
#include "checkerboard.h"
#include "intrinsics.h"

int main() {
  cv::VideoCapture cap(0);

  if (!cap.isOpened()) {
    std::cerr << "Error: could not open camera\n";
    return -1;
  }

  cv::Size patternSize(9, 6); // internal corners

  cv::Mat frame;
  std::vector<cv::Point2f> corners;

  std::vector<cv::Point2f> last_good_corners;
  bool have_last_good = false;

  std::vector<std::vector<cv::Point2f>> corner_list;
  std::vector<std::vector<cv::Vec3f>> point_list;

  cv::Mat cameraMatrix, distCoeffs;
  std::vector<cv::Mat> rvecs, tvecs;
  double reprojectionError = -1.0;
  bool calibrated = false;

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
    } else {
      cv::putText(frame, "Checkerboard not found", cv::Point(20, 30),
                  cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
    }

    std::string saved_msg =
        "Saved frames: " + std::to_string(corner_list.size());
    cv::putText(frame, saved_msg, cv::Point(20, 65), cv::FONT_HERSHEY_SIMPLEX,
                0.8, cv::Scalar(255, 255, 0), 2);

    if (calibrated) {
      std::string err_msg =
          "Reproj error: " + std::to_string(reprojectionError);
      cv::putText(frame, err_msg, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX,
                  0.8, cv::Scalar(255, 0, 255), 2);
    }

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
      } else {
        std::cout << "Failed to save calibration frame\n";
      }

    } else if (key == 'c') {
      int rc = calibrateFromPoints(point_list, corner_list, frame.size(),
                                   cameraMatrix, distCoeffs, rvecs, tvecs,
                                   reprojectionError);

      if (rc == 0) {
        calibrated = true;
        std::cout << "Calibration successful\n";
      } else {
        calibrated = false;
        std::cout << "Calibration failed\n";
      }

    } else if (key == 'w') {
      if (!calibrated) {
        std::cout << "No calibration available to write\n";
        continue;
      }

      int rc =
          writeIntrinsics("data/calibration.yml", cameraMatrix, distCoeffs);

      if (rc == 0) {
        std::cout << "Wrote calibration to data/calibration.yml\n";
      } else {
        std::cout << "Failed to write calibration file\n";
      }
    }
  }

  cap.release();
  cv::destroyAllWindows();

  return 0;
}