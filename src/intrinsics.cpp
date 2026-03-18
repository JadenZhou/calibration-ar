/**
 * intrinsics.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Camera calibration and intrinsic parameter I/O utilities.
 */

#include "intrinsics.h"

#include <iostream>

int calibrateFromPoints(
    const std::vector<std::vector<cv::Vec3f>> &point_list,
    const std::vector<std::vector<cv::Point2f>> &corner_list,
    const cv::Size &imageSize, cv::Mat &cameraMatrix, cv::Mat &distCoeffs,
    std::vector<cv::Mat> &rvecs, std::vector<cv::Mat> &tvecs,
    double &reprojectionError) {

  if (point_list.size() < 5 || corner_list.size() < 5) {
    std::cerr << "Error: need at least 5 saved calibration frames\n";
    return -1;
  }

  if (point_list.size() != corner_list.size()) {
    std::cerr << "Error: point_list and corner_list size mismatch\n";
    return -2;
  }

  // 3x3 matrix of doubles, as required
  cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
  cameraMatrix.at<double>(0, 2) = imageSize.width / 2.0;
  cameraMatrix.at<double>(1, 2) = imageSize.height / 2.0;

  // Use 5 distortion parameters to start
  distCoeffs = cv::Mat::zeros(5, 1, CV_64F);

  std::cout << "\nInitial camera matrix:\n" << cameraMatrix << "\n";
  std::cout << "Initial distortion coefficients:\n" << distCoeffs << "\n";

  int flags = cv::CALIB_FIX_ASPECT_RATIO;

  reprojectionError =
      cv::calibrateCamera(point_list, corner_list, imageSize, cameraMatrix,
                          distCoeffs, rvecs, tvecs, flags);

  std::cout << "\nCalibrated camera matrix:\n" << cameraMatrix << "\n";
  std::cout << "Calibrated distortion coefficients:\n" << distCoeffs << "\n";
  std::cout << "Final reprojection error: " << reprojectionError << "\n";

  return 0;
}

int writeIntrinsics(const std::string &filename, const cv::Mat &cameraMatrix,
                    const cv::Mat &distCoeffs) {
  cv::FileStorage fs(filename, cv::FileStorage::WRITE);

  if (!fs.isOpened()) {
    std::cerr << "Error: could not open file for writing: " << filename << "\n";
    return -1;
  }

  fs << "camera_matrix" << cameraMatrix;
  fs << "distortion_coefficients" << distCoeffs;
  fs.release();

  return 0;
}