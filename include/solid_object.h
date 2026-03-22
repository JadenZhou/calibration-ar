/**
 * solid_object.h
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Solid 3D object utilities (GL-shaded).
 */

#ifndef SOLID_OBJECT_H
#define SOLID_OBJECT_H

#include <opencv2/opencv.hpp>
#include <vector>

/**
 * SolidType
 * Enum representing available GL-shaded solid objects.
 */
enum class SolidType { HOUSE, TEAPOT };

/**
 * nextSolidType / solidTypeName
 * Cycle through and label solid objects.
 */
SolidType nextSolidType(SolidType type);
const char *solidTypeName(SolidType type);

/**
 * renderSolidGL
 * Renders the given solid object using OpenGL fixed-function pipeline.
 * Assumes GL projection and modelview matrices are already set.
 *
 * Arguments:
 * - type: which solid object to render
 */
void renderSolidGL(SolidType type);

// ── Legacy OpenCV drawing (kept for non-GL fallback) ─────────

std::vector<cv::Point3f> makeSolidHousePoints();
std::vector<std::vector<int>> makeSolidHouseFaces();

int projectSolidObject(const std::vector<cv::Point3f> &objectPoints,
                       const cv::Mat &rvec, const cv::Mat &tvec,
                       const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs,
                       std::vector<cv::Point2f> &imagePoints);

int drawSolidObject(cv::Mat &frame,
                    const std::vector<cv::Point3f> &objectPoints,
                    const std::vector<cv::Point2f> &imagePoints,
                    const std::vector<std::vector<int>> &faces,
                    const cv::Mat &rvec, const cv::Mat &tvec);

#endif