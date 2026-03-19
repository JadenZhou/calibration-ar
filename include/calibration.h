/**
 * calibration.h
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Calibration data utilities.
 */

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <opencv2/opencv.hpp>
#include <vector>

/**
 * buildPointSet
 * Builds the 3D checkerboard world coordinates for one calibration image.
 *
 * Arguments:
 * - patternSize: number of internal corners (cols, rows)
 *
 * Returns:
 * - vector of 3D world points in checkerboard coordinates
 */
std::vector<cv::Vec3f> buildPointSet(const cv::Size &patternSize);

/**
 * saveCalibrationFrame
 * Saves one detected checkerboard corner set, its matching 3D point set,
 * and a copy of the corresponding image.
 *
 * Arguments:
 * - frame: calibration image to save
 * - corners: detected 2D checkerboard corners from that image
 * - patternSize: number of internal corners (cols, rows)
 * - corner_list: accumulated list of 2D corner sets
 * - point_list: accumulated list of 3D point sets
 * - image_list: accumulated list of saved calibration images
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int saveCalibrationFrame(const cv::Mat &frame,
                         const std::vector<cv::Point2f> &corners,
                         const cv::Size &patternSize,
                         std::vector<std::vector<cv::Point2f>> &corner_list,
                         std::vector<std::vector<cv::Vec3f>> &point_list,
                         std::vector<cv::Mat> &image_list);

#endif