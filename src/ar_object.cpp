/**
 * ar_object.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Virtual 3D banana object definitions and drawing utilities.
 */

#include "ar_object.h"

#include <iostream>

static std::vector<cv::Point3f> makeBananaPoints() {
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

static std::vector<std::pair<int, int>> makeBananaEdges() {
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

/**
 * makePandaPoints
 * Builds a floating wireframe panda in world coordinates.
 */
static std::vector<cv::Point3f> makePandaPoints() {
  std::vector<cv::Point3f> pts;

  const float s = 1.2f;
  const float cx = 4.0f;
  const float cy = -2.5f;
  const float cz = -2.0f;

  // Head square: 0..3
  pts.push_back(
      cv::Point3f(cx + s * (-0.9f), cy + s * (1.6f), cz + s * (0.0f))); // 0
  pts.push_back(
      cv::Point3f(cx + s * (0.9f), cy + s * (1.6f), cz + s * (0.0f))); // 1
  pts.push_back(
      cv::Point3f(cx + s * (0.9f), cy + s * (0.0f), cz + s * (0.0f))); // 2
  pts.push_back(
      cv::Point3f(cx + s * (-0.9f), cy + s * (0.0f), cz + s * (0.0f))); // 3

  // Ears: 4..7
  pts.push_back(
      cv::Point3f(cx + s * (-1.2f), cy + s * (2.1f), cz + s * (-0.1f))); // 4
  pts.push_back(
      cv::Point3f(cx + s * (-0.5f), cy + s * (2.4f), cz + s * (-0.2f))); // 5
  pts.push_back(
      cv::Point3f(cx + s * (0.5f), cy + s * (2.4f), cz + s * (-0.2f))); // 6
  pts.push_back(
      cv::Point3f(cx + s * (1.2f), cy + s * (2.1f), cz + s * (-0.1f))); // 7

  // Body square: 8..11
  pts.push_back(
      cv::Point3f(cx + s * (-1.2f), cy + s * (-0.2f), cz + s * (0.0f))); // 8
  pts.push_back(
      cv::Point3f(cx + s * (1.2f), cy + s * (-0.2f), cz + s * (0.0f))); // 9
  pts.push_back(
      cv::Point3f(cx + s * (1.2f), cy + s * (-2.2f), cz + s * (0.0f))); // 10
  pts.push_back(
      cv::Point3f(cx + s * (-1.2f), cy + s * (-2.2f), cz + s * (0.0f))); // 11

  // Arms: 12..15
  pts.push_back(
      cv::Point3f(cx + s * (-1.8f), cy + s * (-0.4f), cz + s * (0.1f))); // 12
  pts.push_back(
      cv::Point3f(cx + s * (-1.8f), cy + s * (-1.6f), cz + s * (0.1f))); // 13
  pts.push_back(
      cv::Point3f(cx + s * (1.8f), cy + s * (-0.4f), cz + s * (0.1f))); // 14
  pts.push_back(
      cv::Point3f(cx + s * (1.8f), cy + s * (-1.6f), cz + s * (0.1f))); // 15

  // Legs: 16..19
  pts.push_back(
      cv::Point3f(cx + s * (-0.8f), cy + s * (-2.2f), cz + s * (0.1f))); // 16
  pts.push_back(
      cv::Point3f(cx + s * (-0.8f), cy + s * (-3.3f), cz + s * (0.1f))); // 17
  pts.push_back(
      cv::Point3f(cx + s * (0.8f), cy + s * (-2.2f), cz + s * (0.1f))); // 18
  pts.push_back(
      cv::Point3f(cx + s * (0.8f), cy + s * (-3.3f), cz + s * (0.1f))); // 19

  // Eye patches: 20..23
  pts.push_back(
      cv::Point3f(cx + s * (-0.55f), cy + s * (1.10f), cz + s * (0.12f))); // 20
  pts.push_back(
      cv::Point3f(cx + s * (-0.20f), cy + s * (0.75f), cz + s * (0.12f))); // 21
  pts.push_back(
      cv::Point3f(cx + s * (0.20f), cy + s * (0.75f), cz + s * (0.12f))); // 22
  pts.push_back(
      cv::Point3f(cx + s * (0.55f), cy + s * (1.10f), cz + s * (0.12f))); // 23

  // Nose / mouth: 24..27
  pts.push_back(
      cv::Point3f(cx + s * (0.00f), cy + s * (0.55f), cz + s * (0.18f))); // 24
  pts.push_back(
      cv::Point3f(cx + s * (-0.18f), cy + s * (0.30f), cz + s * (0.18f))); // 25
  pts.push_back(
      cv::Point3f(cx + s * (0.18f), cy + s * (0.30f), cz + s * (0.18f))); // 26
  pts.push_back(
      cv::Point3f(cx + s * (0.00f), cy + s * (0.05f), cz + s * (0.18f))); // 27

  return pts;
}

/**
 * makePandaEdges
 * Defines line segments between panda vertices.
 */
static std::vector<std::pair<int, int>> makePandaEdges() {
  return {// head
          {0, 1},
          {1, 2},
          {2, 3},
          {3, 0},

          // ears
          {0, 4},
          {4, 5},
          {5, 0},
          {1, 7},
          {7, 6},
          {6, 1},

          // body
          {8, 9},
          {9, 10},
          {10, 11},
          {11, 8},

          // neck/body connection
          {3, 8},
          {2, 9},

          // arms
          {8, 12},
          {12, 13},
          {13, 11},
          {9, 14},
          {14, 15},
          {15, 10},

          // legs
          {11, 16},
          {16, 17},
          {10, 18},
          {18, 19},

          // eye patches
          {20, 21},
          {22, 23},

          // nose/mouth
          {21, 24},
          {24, 22},
          {24, 25},
          {24, 26},
          {25, 27},
          {26, 27}};
}

std::vector<cv::Point3f> makePoints(ObjectType type) {
  switch (type) {
  case ObjectType::BANANA:
    return makeBananaPoints(); // your existing function
  case ObjectType::PANDA:
    return makePandaPoints();
  default:
    return {};
  }
}

std::vector<std::pair<int, int>> makeEdges(ObjectType type) {
  switch (type) {
  case ObjectType::BANANA:
    return makeBananaEdges();
  case ObjectType::PANDA:
    return makePandaEdges();
  default:
    return {};
  }
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

int drawObject(cv::Mat &frame, ObjectType type,
               const std::vector<cv::Point2f> &imagePoints,
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

    if (type == ObjectType::BANANA) {
      if (i < 24) {
        color = cv::Scalar(255, 255, 0); // cyan slice diamonds
      } else if (i < 44) {
        color = cv::Scalar(0, 255, 255); // yellow rails
      } else {
        color = cv::Scalar(30, 60, 120); // stem/tip
      }
    } else if (type == ObjectType::PANDA) {
      if (i < 10) {
        color = cv::Scalar(255, 255, 255); // white head/ears
      } else if (i < 28) {
        color = cv::Scalar(180, 180, 180); // gray body/limbs
      } else {
        color = cv::Scalar(0, 0, 0); // black face details
      }
    } else {
      color = cv::Scalar(0, 255, 0);
    }

    cv::line(frame, p1, p2, color, 2, cv::LINE_AA);
  }

  return 0;
}