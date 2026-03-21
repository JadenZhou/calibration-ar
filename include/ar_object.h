/**
 * ar_object.h
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Virtual 3D object definitions and drawing utilities.
 */

#ifndef AR_OBJECT_H
#define AR_OBJECT_H

#include <opencv2/opencv.hpp>
#include <vector>

/**
 * makeHousePoints
 * Builds a simple floating wireframe house in board coordinates.
 *
 * Returns:
 * - vector of 3D points
 */
std::vector<cv::Point3f> makeHousePoints();

/**
 * makeHouseEdges
 * Builds edge list for the wireframe house.
 *
 * Returns:
 * - vector of vertex index pairs
 */
std::vector<std::pair<int, int>> makeHouseEdges();

/**
 * projectObject
 * Projects 3D object points into image coordinates.
 *
 * Arguments:
 * - objectPoints: 3D object vertices
 * - rvec: pose rotation
 * - tvec: pose translation
 * - cameraMatrix: intrinsic matrix
 * - distCoeffs: distortion coefficients
 * - imagePoints: output projected 2D points
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int projectObject(const std::vector<cv::Point3f> &objectPoints,
                  const cv::Mat &rvec, const cv::Mat &tvec,
                  const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs,
                  std::vector<cv::Point2f> &imagePoints);

/**
 * drawObject
 * Draws the projected wireframe object using multiple colors.
 *
 * Arguments:
 * - frame: image to draw on
 * - imagePoints: projected 2D vertices
 * - edges: list of vertex index pairs
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int drawObject(cv::Mat &frame, const std::vector<cv::Point2f> &imagePoints,
               const std::vector<std::pair<int, int>> &edges);

#endif