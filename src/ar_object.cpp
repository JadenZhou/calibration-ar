/**
 * ar_object.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Virtual 3D object definitions and drawing utilities implementation.
 */

#include "ar_object.h"

#include <iostream>

std::vector<cv::Point3f> makeHousePoints() {
  std::vector<cv::Point3f> pts;

  // Base rectangle on the board
  pts.push_back(cv::Point3f(0.0f, 0.0f, 0.0f));  // 0
  pts.push_back(cv::Point3f(2.0f, 0.0f, 0.0f));  // 1
  pts.push_back(cv::Point3f(2.0f, -2.0f, 0.0f)); // 2
  pts.push_back(cv::Point3f(0.0f, -2.0f, 0.0f)); // 3

  // Top of walls, floating above board
  pts.push_back(cv::Point3f(0.0f, 0.0f, -2.0f));  // 4
  pts.push_back(cv::Point3f(2.0f, 0.0f, -2.0f));  // 5
  pts.push_back(cv::Point3f(2.0f, -2.0f, -2.0f)); // 6
  pts.push_back(cv::Point3f(0.0f, -2.0f, -2.0f)); // 7

  // Roof ridge
  pts.push_back(cv::Point3f(1.0f, 0.0f, -3.0f));  // 8
  pts.push_back(cv::Point3f(1.0f, -2.0f, -3.0f)); // 9

  return pts;
}

std::vector<std::pair<int, int>> makeHouseEdges() {
  return {// base
          {0, 1},
          {1, 2},
          {2, 3},
          {3, 0},

          // walls
          {0, 4},
          {1, 5},
          {2, 6},
          {3, 7},

          // top rectangle
          {4, 5},
          {5, 6},
          {6, 7},
          {7, 4},

          // roof slopes
          {4, 8},
          {5, 8},
          {7, 9},
          {6, 9},

          // ridge
          {8, 9}};
}

int projectObject(const std::vector<cv::Point3f> &objectPoints,
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

int drawObject(cv::Mat &frame, const std::vector<cv::Point2f> &imagePoints,
               const std::vector<std::pair<int, int>> &edges) {
  if (imagePoints.empty()) {
    std::cerr << "Error: imagePoints is empty\n";
    return -1;
  }

  for (size_t i = 0; i < edges.size(); i++) {
    int a = edges[i].first;
    int b = edges[i].second;

    if (a < 0 || b < 0 || a >= (int)imagePoints.size() ||
        b >= (int)imagePoints.size()) {
      std::cerr << "Error: edge index out of bounds\n";
      return -2;
    }

    cv::Point p1((int)imagePoints[a].x, (int)imagePoints[a].y);
    cv::Point p2((int)imagePoints[b].x, (int)imagePoints[b].y);

    // Use different colors by edge group
    cv::Scalar color;
    if (i < 4) {
      color = cv::Scalar(0, 255, 255); // yellow base
    } else if (i < 12) {
      color = cv::Scalar(255, 0, 255); // magenta walls/top
    } else {
      color = cv::Scalar(255, 255, 0); // cyan roof
    }

    cv::line(frame, p1, p2, color, 3);
  }

  return 0;
}