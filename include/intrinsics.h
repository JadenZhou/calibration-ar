/**
 * intrinsics.h
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Camera calibration and intrinsic parameter I/O utilities.
 */

#ifndef INTRINSICS_H
#define INTRINSICS_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

/**
 * calibrateFromPoints
 * Runs camera calibration from saved 3D-2D correspondences.
 *
 * Arguments:
 * - point_list: list of 3D world point sets
 * - corner_list: list of 2D image point sets
 * - imageSize: size of calibration images
 * - cameraMatrix: 3x3 intrinsic camera matrix (input/output)
 * - distCoeffs: distortion coefficients (output)
 * - rvecs: output rotation vectors for each calibration image
 * - tvecs: output translation vectors for each calibration image
 * - reprojectionError: output reprojection error
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int calibrateFromPoints(
    const std::vector<std::vector<cv::Vec3f>> &point_list,
    const std::vector<std::vector<cv::Point2f>> &corner_list,
    const cv::Size &imageSize, cv::Mat &cameraMatrix, cv::Mat &distCoeffs,
    std::vector<cv::Mat> &rvecs, std::vector<cv::Mat> &tvecs,
    double &reprojectionError);

/**
 * writeIntrinsics
 * Writes the camera matrix and distortion coefficients to a file.
 *
 * Arguments:
 * - filename: output yaml/xml file
 * - cameraMatrix: intrinsic matrix
 * - distCoeffs: distortion coefficients
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int writeIntrinsics(const std::string &filename, const cv::Mat &cameraMatrix,
                    const cv::Mat &distCoeffs);

#endif