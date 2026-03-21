/**
 * features_utils.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Feature detection utilities implementation.
 */

#include "features.h"

#include <iostream>

int detectORBFeatures(const cv::Mat &frame, int maxFeatures, int fastThreshold,
                      std::vector<cv::KeyPoint> &keypoints,
                      cv::Mat &descriptors) {
  if (frame.empty()) {
    std::cerr << "Error: empty frame\n";
    return -1;
  }

  cv::Mat gray;
  cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

  cv::Ptr<cv::ORB> orb = cv::ORB::create(maxFeatures);
  orb->setFastThreshold(fastThreshold);

  orb->detectAndCompute(gray, cv::noArray(), keypoints, descriptors);

  return 0;
}

int drawFeaturePoints(cv::Mat &frame,
                      const std::vector<cv::KeyPoint> &keypoints) {
  if (frame.empty()) {
    std::cerr << "Error: empty frame\n";
    return -1;
  }

  cv::drawKeypoints(frame, keypoints, frame, cv::Scalar(0, 255, 0),
                    cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

  return 0;
}