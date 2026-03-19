/**
 * image_io.h
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Utilities for writing saved calibration images to disk.
 */

#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include <opencv2/opencv.hpp>

#include <string>
#include <vector>

/**
 * writeSavedImages
 * Writes saved calibration images to disk.
 *
 * Arguments:
 * - dir: output directory path
 * - image_list: list of saved calibration images
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int writeSavedImages(const std::string &dir,
                     const std::vector<cv::Mat> &image_list);

#endif