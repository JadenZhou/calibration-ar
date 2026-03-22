/**
 * solid_object.h
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Solid 3D object utilities (filled faces).
 */

#ifndef SOLID_OBJECT_H
#define SOLID_OBJECT_H

#include <opencv2/opencv.hpp>
#include <vector>

/**
 * makeSolidHousePoints
 * Builds a simple 3D house in world coordinates.
 *
 * Returns:
 * - vector of 3D points
 */
std::vector<cv::Point3f> makeSolidHousePoints();

/**
 * makeSolidHouseFaces
 * Defines polygon faces using vertex indices.
 *
 * Returns:
 * - vector of faces (each face is a list of indices)
 */
std::vector<std::vector<int>> makeSolidHouseFaces();

/**
 * projectSolidObject
 * Projects 3D object points into image coordinates.
 *
 * Arguments:
 * - objectPoints: 3D object vertices
 * - rvec: rotation vector
 * - tvec: translation vector
 * - cameraMatrix: intrinsic matrix
 * - distCoeffs: distortion coefficients
 * - imagePoints: output projected points
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int projectSolidObject(const std::vector<cv::Point3f> &objectPoints,
                       const cv::Mat &rvec, const cv::Mat &tvec,
                       const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs,
                       std::vector<cv::Point2f> &imagePoints);

/**
 * drawSolidObject
 * Draws a filled 3D object using projected points.
 * Uses a simple depth ordering (painter's algorithm).
 *
 * Arguments:
 * - frame: image to draw on
 * - objectPoints: 3D object vertices
 * - imagePoints: projected 2D points
 * - faces: list of polygon faces
 * - rvec: rotation vector
 * - tvec: translation vector
 *
 * Returns:
 * - 0 on success, non-zero on error
 */
int drawSolidObject(cv::Mat &frame,
                    const std::vector<cv::Point3f> &objectPoints,
                    const std::vector<cv::Point2f> &imagePoints,
                    const std::vector<std::vector<int>> &faces,
                    const cv::Mat &rvec, const cv::Mat &tvec);

/**
 * computeFaceNormal
 * Computes the outward-facing normal for a polygon face.
 *
 * Arguments:
 * - pts: all object vertices
 * - face: vertex indices defining the face
 *
 * Returns:
 * - unit normal vector
 */
cv::Point3f computeFaceNormal(const std::vector<cv::Point3f> &pts,
                              const std::vector<int> &face);

/**
 * renderSolidHouseGL
 * Renders the solid house using OpenGL with lighting and filled polygons.
 * Call this after setting GL projection and modelview matrices.
 */
void renderSolidHouseGL();

#endif