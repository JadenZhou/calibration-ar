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

int projectAxes(float axisLength, const cv::Mat &rvec, const cv::Mat &tvec,
                const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs,
                std::vector<cv::Point2f> &imagePoints) {
  std::vector<cv::Point3f> axisPoints;

  // origin, +x, +y, +z
  // z is negative so it comes "out" of the board toward the camera
  axisPoints.push_back(cv::Point3f(0.0f, 0.0f, 0.0f));
  axisPoints.push_back(cv::Point3f(axisLength, 0.0f, 0.0f));
  axisPoints.push_back(cv::Point3f(0.0f, -axisLength, 0.0f));
  axisPoints.push_back(cv::Point3f(0.0f, 0.0f, axisLength));

  cv::projectPoints(axisPoints, rvec, tvec, cameraMatrix, distCoeffs,
                    imagePoints);

  if (imagePoints.size() != 4) {
    std::cerr << "Error: expected 4 projected axis points, got "
              << imagePoints.size() << "\n";
    return -1;
  }

  return 0;
}

int drawAxes(cv::Mat &frame, const std::vector<cv::Point2f> &imagePoints) {
  if (imagePoints.size() != 4) {
    std::cerr << "Error: drawAxes requires 4 image points\n";
    return -1;
  }

  const cv::Point origin = imagePoints[0];
  const cv::Point xpt = imagePoints[1];
  const cv::Point ypt = imagePoints[2];
  const cv::Point zpt = imagePoints[3];

  cv::line(frame, origin, xpt, cv::Scalar(0, 0, 255), 3); // X = red
  cv::line(frame, origin, ypt, cv::Scalar(0, 255, 0), 3); // Y = green
  cv::line(frame, origin, zpt, cv::Scalar(255, 0, 0), 3); // Z = blue

  cv::circle(frame, origin, 5, cv::Scalar(255, 255, 255), -1);

  return 0;
}