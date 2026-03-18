/**
 * calibration_utils.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Calibration data utilities implementation.
 */

#include "calibration.h"

#include <iostream>

std::vector<cv::Vec3f> buildPointSet(const cv::Size &patternSize) {
  std::vector<cv::Vec3f> point_set;

  for (int r = 0; r < patternSize.height; r++) {
    for (int c = 0; c < patternSize.width; c++) {
      point_set.push_back(cv::Vec3f((float)c, (float)(-r), 0.0f));
    }
  }

  return point_set;
}

int saveCalibrationFrame(const std::vector<cv::Point2f> &corners,
                         const cv::Size &patternSize,
                         std::vector<std::vector<cv::Point2f>> &corner_list,
                         std::vector<std::vector<cv::Vec3f>> &point_list) {
  const int expected = patternSize.width * patternSize.height;

  if ((int)corners.size() != expected) {
    std::cerr << "Error: expected " << expected << " corners, got "
              << corners.size() << "\n";
    return -1;
  }

  std::vector<cv::Vec3f> point_set = buildPointSet(patternSize);

  if (point_set.size() != corners.size()) {
    std::cerr << "Error: point_set size does not match corners size\n";
    return -2;
  }

  corner_list.push_back(corners);
  point_list.push_back(point_set);

  return 0;
}