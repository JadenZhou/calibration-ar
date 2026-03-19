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

/**
 * projectAxes
 * Projects 3D axes from board coordinates into the image.
 *
 * Arguments:
 * - axisLength: length of each axis in board units
 * - rvec: board pose rotation
 * - tvec: board pose translation
 * - cameraMatrix: intrinsic matrix
 * - distCoeffs: distortion coefficients
 * - imagePoints: output projected 2D points
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int projectAxes(float axisLength, const cv::Mat &rvec, const cv::Mat &tvec,
                const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs,
                std::vector<cv::Point2f> &imagePoints);

/**
 * drawAxes
 * Draws projected 3D axes on the frame.
 *
 * Convention:
 * - X axis: red
 * - Y axis: green
 * - Z axis: blue
 *
 * Arguments:
 * - frame: image to draw on
 * - imagePoints: projected axis points [origin, x, y, z]
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int drawAxes(cv::Mat &frame, const std::vector<cv::Point2f> &imagePoints);

#endif