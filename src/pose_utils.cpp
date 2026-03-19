/**
 * pose_utils.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Pose estimation utilities implementation.
 */

#include "pose.h"

#include <iostream>

int readIntrinsics(const std::string &filename, cv::Mat &cameraMatrix,
                   cv::Mat &distCoeffs) {
  cv::FileStorage fs(filename, cv::FileStorage::READ);

  if (!fs.isOpened()) {
    std::cerr << "Error: could not open calibration file: " << filename << "\n";
    return -1;
  }

  fs["camera_matrix"] >> cameraMatrix;
  fs["distortion_coefficients"] >> distCoeffs;
  fs.release();

  if (cameraMatrix.empty() || distCoeffs.empty()) {
    std::cerr << "Error: missing calibration data in file\n";
    return -2;
  }

  return 0;
}

int estimatePose(const std::vector<cv::Vec3f> &point_set,
                 const std::vector<cv::Point2f> &corners,
                 const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs,
                 cv::Mat &rvec, cv::Mat &tvec) {
  if (point_set.empty() || corners.empty()) {
    std::cerr << "Error: empty point_set or corners\n";
    return -1;
  }

  if (point_set.size() != corners.size()) {
    std::cerr << "Error: point_set and corners size mismatch\n";
    return -2;
  }

  bool ok =
      cv::solvePnP(point_set, corners, cameraMatrix, distCoeffs, rvec, tvec);

  if (!ok) {
    std::cerr << "Error: solvePnP failed\n";
    return -3;
  }

  return 0;
}