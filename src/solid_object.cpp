/**
 * solid_object.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Solid 3D object utilities implementation.
 */

#include "solid_object.h"

#include <algorithm>
#include <iostream>

/**
 * makeSolidHousePoints
 * Builds a simple 3D house in world coordinates.
 */
std::vector<cv::Point3f> makeSolidHousePoints() {
  return {{0.0f, 0.0f, 0.0f},   {2.0f, 0.0f, 0.0f},
          {2.0f, -2.0f, 0.0f},  {0.0f, -2.0f, 0.0f},

          {0.0f, 0.0f, -2.0f},  {2.0f, 0.0f, -2.0f},
          {2.0f, -2.0f, -2.0f}, {0.0f, -2.0f, -2.0f},

          {1.0f, 0.0f, -3.0f},  {1.0f, -2.0f, -3.0f}};
}

/**
 * makeSolidHouseFaces
 * Defines polygon faces using vertex indices.
 */
std::vector<std::vector<int>> makeSolidHouseFaces() {
  return {{0, 1, 2, 3}, {0, 1, 5, 4}, {1, 2, 6, 5}, {2, 3, 7, 6}, {3, 0, 4, 7},
          {4, 5, 8},    {7, 6, 9},    {4, 8, 9, 7}, {5, 6, 9, 8}};
}

/**
 * projectSolidObject
 * Projects 3D object points into image coordinates.
 */
int projectSolidObject(const std::vector<cv::Point3f> &objectPoints,
                       const cv::Mat &rvec, const cv::Mat &tvec,
                       const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs,
                       std::vector<cv::Point2f> &imagePoints) {
  if (objectPoints.empty()) {
    std::cerr << "Error: objectPoints is empty\n";
    return -1;
  }

  cv::projectPoints(objectPoints, rvec, tvec, cameraMatrix, distCoeffs,
                    imagePoints);

  if (imagePoints.size() != objectPoints.size()) {
    std::cerr << "Error: projection size mismatch\n";
    return -2;
  }

  return 0;
}

/**
 * drawSolidObject
 * Draws a filled 3D object using projected points.
 * Uses a simple depth ordering (painter's algorithm).
 */
int drawSolidObject(cv::Mat &frame,
                    const std::vector<cv::Point3f> &objectPoints,
                    const std::vector<cv::Point2f> &imagePoints,
                    const std::vector<std::vector<int>> &faces,
                    const cv::Mat &rvec, const cv::Mat &tvec) {
  if (imagePoints.empty()) {
    std::cerr << "Error: imagePoints is empty\n";
    return -1;
  }

  struct FaceInfo {
    std::vector<int> idx;
    double depth;
    cv::Scalar color;
  };

  std::vector<cv::Point3f> camPts;

  cv::Mat R;
  cv::Rodrigues(rvec, R);

  // transform points into camera space
  for (const auto &p : objectPoints) {
    cv::Mat X = (cv::Mat_<double>(3, 1) << p.x, p.y, p.z);
    cv::Mat Xc = R * X + tvec;

    camPts.push_back(cv::Point3f((float)Xc.at<double>(0, 0),
                                 (float)Xc.at<double>(1, 0),
                                 (float)Xc.at<double>(2, 0)));
  }

  std::vector<FaceInfo> ordered;

  std::vector<cv::Scalar> palette = {
      cv::Scalar(180, 180, 180), cv::Scalar(80, 170, 255),
      cv::Scalar(80, 220, 120),  cv::Scalar(255, 170, 80),
      cv::Scalar(220, 120, 220), cv::Scalar(80, 80, 220),
      cv::Scalar(80, 80, 200),   cv::Scalar(120, 120, 255),
      cv::Scalar(120, 120, 235)};

  // compute average depth per face
  for (size_t i = 0; i < faces.size(); i++) {
    double avgz = 0.0;
    for (int idx : faces[i]) {
      avgz += camPts[idx].z;
    }
    avgz /= faces[i].size();

    ordered.push_back({faces[i], avgz, palette[i % palette.size()]});
  }

  // sort by depth (far to near)
  std::sort(
      ordered.begin(), ordered.end(),
      [](const FaceInfo &a, const FaceInfo &b) { return a.depth > b.depth; });

  // draw faces
  for (const auto &face : ordered) {
    std::vector<cv::Point> poly;

    for (int idx : face.idx) {
      poly.emplace_back((int)imagePoints[idx].x, (int)imagePoints[idx].y);
    }

    if (poly.size() >= 3) {
      cv::fillConvexPoly(frame, poly, face.color, cv::LINE_AA);
      cv::polylines(frame, poly, true, cv::Scalar(0, 0, 0), 2, cv::LINE_AA);
    }
  }

  return 0;
}