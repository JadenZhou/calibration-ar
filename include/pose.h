/**
 * pose.h
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Pose estimation utilities.
 */

#ifndef POSE_H
#define POSE_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

/**
 * readIntrinsics
 * Reads camera intrinsics from a yaml/xml file.
 *
 * Arguments:
 * - filename: input calibration file
 * - cameraMatrix: output intrinsic matrix
 * - distCoeffs: output distortion coefficients
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int readIntrinsics(const std::string &filename, cv::Mat &cameraMatrix,
                   cv::Mat &distCoeffs);

/**
 * estimatePose
 * Estimates checkerboard pose from 3D board points and detected 2D corners.
 *
 * Arguments:
 * - point_set: 3D checkerboard points in board coordinates
 * - corners: detected 2D checkerboard corners
 * - cameraMatrix: intrinsic matrix
 * - distCoeffs: distortion coefficients
 * - rvec: output rotation vector
 * - tvec: output translation vector
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int estimatePose(const std::vector<cv::Vec3f> &point_set,
                 const std::vector<cv::Point2f> &corners,
                 const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs,
                 cv::Mat &rvec, cv::Mat &tvec);

#endif