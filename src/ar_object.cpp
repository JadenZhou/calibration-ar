/**
 * ar_object.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Virtual 3D banana object definitions and drawing utilities.
 */

#include "ar_object.h"

#include <iostream>

std::vector<cv::Point3f> makeBananaPoints() {
  std::vector<cv::Point3f> pts;

  const float s = 3.2f;
  const float cx = 4.0f;
  const float cy = -2.5f;
  const float cz = 5.0f;

  // centerline of banana
  std::vector<cv::Point3f> c = {
      {cx + s * (-2.0f), cy + s * (1.0f), cz + s * (0.0f)},
      {cx + s * (-1.2f), cy + s * (0.8f), cz + s * (-0.2f)},
      {cx + s * (-0.4f), cy + s * (0.5f), cz + s * (-0.4f)},
      {cx + s * (0.4f), cy + s * (0.1f), cz + s * (-0.6f)},
      {cx + s * (1.2f), cy + s * (-0.4f), cz + s * (-0.4f)},
      {cx + s * (2.0f), cy + s * (-1.0f), cz + s * (-0.1f)}};

  // thickness profile: bigger in middle, tapered at ends
  std::vector<float> h = {0.18f, 0.26f, 0.34f,
                          0.34f, 0.26f, 0.16f}; // vertical half-size
  std::vector<float> d = {0.12f, 0.18f, 0.24f,
                          0.24f, 0.18f, 0.10f}; // depth half-size

  for (int i = 0; i < (int)c.size(); i++) {
    float hh = s * h[i];
    float dd = s * d[i];

    // top, right, bottom, left  (diamond cross-section)
    pts.push_back(cv::Point3f(c[i].x, c[i].y - hh, c[i].z)); // 4*i + 0
    pts.push_back(cv::Point3f(c[i].x, c[i].y, c[i].z + dd)); // 4*i + 1
    pts.push_back(cv::Point3f(c[i].x, c[i].y + hh, c[i].z)); // 4*i + 2
    pts.push_back(cv::Point3f(c[i].x, c[i].y, c[i].z - dd)); // 4*i + 3
  }

  // stem
  pts.push_back(
      cv::Point3f(cx + s * (-2.25f), cy + s * (1.15f), cz + s * (0.05f))); // 24
  pts.push_back(cv::Point3f(cx + s * (-2.45f), cy + s * (1.30f),
                            cz + s * (-0.20f))); // 25

  // tip
  pts.push_back(cv::Point3f(cx + s * (2.20f), cy + s * (-1.10f),
                            cz + s * (-0.05f))); // 26
  pts.push_back(cv::Point3f(cx + s * (2.40f), cy + s * (-1.20f),
                            cz + s * (-0.20f))); // 27

  return pts;
}

std::vector<std::pair<int, int>> makeBananaEdges() {
  std::vector<std::pair<int, int>> edges;

  // 6 cross-sections, 4 points each
  for (int i = 0; i < 6; i++) {
    int b = 4 * i;

    // diamond around each slice
    edges.push_back({b + 0, b + 1});
    edges.push_back({b + 1, b + 2});
    edges.push_back({b + 2, b + 3});
    edges.push_back({b + 3, b + 0});
  }

  // connect matching points between neighboring slices
  for (int i = 0; i < 5; i++) {
    int a = 4 * i;
    int b = 4 * (i + 1);

    edges.push_back({a + 0, b + 0}); // top rail
    edges.push_back({a + 1, b + 1}); // right rail
    edges.push_back({a + 2, b + 2}); // bottom rail
    edges.push_back({a + 3, b + 3}); // left rail
  }

  // stem attached to first slice
  edges.push_back({24, 25});
  edges.push_back({24, 0});
  edges.push_back({24, 1});

  // tip attached to last slice
  edges.push_back({26, 27});
  edges.push_back({26, 22});
  edges.push_back({26, 21});

  return edges;
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

    cv::Scalar color;
    if (i < 24) {
      color = cv::Scalar(255, 255, 0); // cyan slice diamonds
    } else if (i < 44) {
      color = cv::Scalar(0, 255, 255); // yellow longitudinal rails
    } else {
      color = cv::Scalar(30, 60, 120); // brown-ish stem/tip
    }

    cv::line(frame, p1, p2, color, 2);
  }

  return 0;
}