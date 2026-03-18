/**
 * checkerboard.h
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Checkerboard detection utilities for calibration.
 */

#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H

#include <opencv2/opencv.hpp>
#include <vector>

/**
 * detectCheckerboard
 * Detects checkerboard corners in an image and refines them.
 *
 * Arguments:
 * - frame: input BGR image
 * - patternSize: number of internal corners (cols, rows)
 * - corners: output vector of 2D points
 *
 * Returns:
 * - true if found, false otherwise
 */
bool detectCheckerboard(const cv::Mat &frame, const cv::Size &patternSize,
                        std::vector<cv::Point2f> &corners);

/**
 * drawCorners
 * Draws detected checkerboard corners onto image.
 */
void drawCorners(cv::Mat &frame, const cv::Size &patternSize,
                 const std::vector<cv::Point2f> &corners, bool found);

#endif