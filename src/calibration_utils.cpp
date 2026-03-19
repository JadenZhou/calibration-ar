/**
 * calibration_utils.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Calibration data utilities for Project 4.
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

int saveCalibrationFrame(const cv::Mat &frame,
                         const std::vector<cv::Point2f> &corners,
                         const cv::Size &patternSize,
                         std::vector<std::vector<cv::Point2f>> &corner_list,
                         std::vector<std::vector<cv::Vec3f>> &point_list,
                         std::vector<cv::Mat> &image_list) {
  const int expected = patternSize.width * patternSize.height;

  if (frame.empty()) {
    std::cerr << "Error: cannot save empty frame\n";
    return -1;
  }

  if ((int)corners.size() != expected) {
    std::cerr << "Error: expected " << expected << " corners, got "
              << corners.size() << "\n";
    return -2;
  }

  std::vector<cv::Vec3f> point_set = buildPointSet(patternSize);

  if (point_set.size() != corners.size()) {
    std::cerr << "Error: point_set size does not match corners size\n";
    return -3;
  }

  corner_list.push_back(corners);
  point_list.push_back(point_set);
  image_list.push_back(frame.clone());

  return 0;
}