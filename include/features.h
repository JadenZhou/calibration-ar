/**
 * features.h
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Feature detection utilities.
 */

#ifndef FEATURES_H
#define FEATURES_H

#include <opencv2/opencv.hpp>
#include <vector>

/**
 * detectORBFeatures
 * Detects ORB keypoints and computes descriptors.
 *
 * Arguments:
 * - frame: input BGR image
 * - maxFeatures: maximum number of ORB features
 * - fastThreshold: FAST threshold for ORB
 * - keypoints: output detected keypoints
 * - descriptors: output descriptors
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int detectORBFeatures(const cv::Mat &frame, int maxFeatures, int fastThreshold,
                      std::vector<cv::KeyPoint> &keypoints,
                      cv::Mat &descriptors);

/**
 * drawFeaturePoints
 * Draws keypoints on the frame.
 *
 * Arguments:
 * - frame: input/output image
 * - keypoints: detected keypoints
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int drawFeaturePoints(cv::Mat &frame,
                      const std::vector<cv::KeyPoint> &keypoints);

#endif