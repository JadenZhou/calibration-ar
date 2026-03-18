/**
 * checkerboard.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Checkerboard detection implementation.
 */

#include "checkerboard.h"

bool detectCheckerboard(const cv::Mat &frame, const cv::Size &patternSize,
                        std::vector<cv::Point2f> &corners) {
  cv::Mat gray;

  // Convert to grayscale
  cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

  // Detect checkerboard corners
  bool found = cv::findChessboardCorners(gray, patternSize, corners,
                                         cv::CALIB_CB_ADAPTIVE_THRESH |
                                             cv::CALIB_CB_NORMALIZE_IMAGE);

  // Refine corner positions
  if (found) {
    cv::cornerSubPix(
        gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
        cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30,
                         0.1));
  }

  return found;
}

void drawCorners(cv::Mat &frame, const cv::Size &patternSize,
                 const std::vector<cv::Point2f> &corners, bool found) {
  cv::drawChessboardCorners(frame, patternSize, corners, found);
}