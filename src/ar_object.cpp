/**
 * ar_object.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Virtual 3D banana object definitions and drawing utilities.
 */

#include "ar_object.h"

#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

  const float s = 3.0f;
  const float cx = 4.0f;
  const float cy = -2.5f;
  const float cz = 3.0f;
  const int N = 8;  // body/head ring resolution
  const int NL = 6; // leg ring resolution

  // Elliptical ring in the YZ plane at position x0.
  // y0 = lateral offset, z0 = height offset
  // rv = vertical (Z) radius, rh = horizontal (Y) radius
  auto ring = [&](float x0, float y0, float z0, float rv, float rh, int n) {
    for (int i = 0; i < n; i++) {
      float a = 2.0f * (float)M_PI * i / n;
      pts.push_back({cx + s * x0, cy + s * (y0 + rh * std::sin(a)),
                     cz + s * (z0 + rv * std::cos(a))});
    }
  };

  // === BODY [0-47]: 6 rings × 8 pts ===
  // Body centerline at Z ≈ 1.7 so paws land near Z ≈ 0
  ring(-1.20f, 0.0f, 1.70f, 0.55f, 0.45f, N); //  0: rump
  ring(-0.60f, 0.0f, 1.75f, 0.70f, 0.55f, N); //  8
  ring(0.00f, 0.0f, 1.78f, 0.78f, 0.60f, N);  // 16: belly (widest)
  ring(0.60f, 0.0f, 1.76f, 0.72f, 0.55f, N);  // 24
  ring(1.00f, 0.0f, 1.70f, 0.60f, 0.48f, N);  // 32: shoulders
  ring(1.35f, 0.0f, 1.70f, 0.42f, 0.35f, N);  // 40: neck base

  // === HEAD [48-87]: 5 rings × 8 pts ===
  // Head tilts upward from neck
  ring(1.55f, 0.0f, 1.90f, 0.40f, 0.35f, N); // 48: neck top
  ring(1.85f, 0.0f, 2.05f, 0.58f, 0.48f, N); // 56: back skull
  ring(2.15f, 0.0f, 2.08f, 0.65f, 0.52f, N); // 64: mid skull
  ring(2.45f, 0.0f, 2.02f, 0.55f, 0.45f, N); // 72: front skull
  ring(2.70f, 0.0f, 1.92f, 0.28f, 0.22f, N); // 80: snout

  // === FRONT-LEFT LEG [88-105]: 3 rings × 6 pts ===
  ring(0.85f, -0.30f, 1.20f, 0.20f, 0.18f, NL); //  88: shoulder
  ring(0.82f, -0.30f, 0.60f, 0.16f, 0.14f, NL); //  94: mid
  ring(0.82f, -0.30f, 0.05f, 0.20f, 0.18f, NL); // 100: paw

  // === FRONT-RIGHT LEG [106-123]: 3 rings × 6 pts ===
  ring(0.85f, 0.30f, 1.20f, 0.20f, 0.18f, NL);
  ring(0.82f, 0.30f, 0.60f, 0.16f, 0.14f, NL);
  ring(0.82f, 0.30f, 0.05f, 0.20f, 0.18f, NL);

  // === BACK-LEFT LEG [124-141]: 3 rings × 6 pts ===
  ring(-0.85f, -0.28f, 1.25f, 0.22f, 0.20f, NL);
  ring(-0.88f, -0.28f, 0.65f, 0.17f, 0.15f, NL);
  ring(-0.88f, -0.28f, 0.10f, 0.21f, 0.19f, NL);

  // === BACK-RIGHT LEG [142-159]: 3 rings × 6 pts ===
  ring(-0.85f, 0.28f, 1.25f, 0.22f, 0.20f, NL);
  ring(-0.88f, 0.28f, 0.65f, 0.17f, 0.15f, NL);
  ring(-0.88f, 0.28f, 0.10f, 0.21f, 0.19f, NL);

  // === LEFT EAR [160-163] (diamond, -Y side, high Z) ===
  pts.push_back(
      {cx + s * 1.95f, cy + s * (-0.35f), cz + s * 2.65f}); // 160: front
  pts.push_back(
      {cx + s * 2.05f, cy + s * (-0.45f), cz + s * 2.92f}); // 161: tip (up)
  pts.push_back(
      {cx + s * 2.25f, cy + s * (-0.35f), cz + s * 2.65f}); // 162: back
  pts.push_back(
      {cx + s * 2.10f, cy + s * (-0.30f), cz + s * 2.52f}); // 163: base

  // === RIGHT EAR [164-167] (diamond, +Y side, high Z) ===
  pts.push_back({cx + s * 1.95f, cy + s * 0.35f, cz + s * 2.65f}); // 164
  pts.push_back({cx + s * 2.05f, cy + s * 0.45f, cz + s * 2.92f}); // 165
  pts.push_back({cx + s * 2.25f, cy + s * 0.35f, cz + s * 2.65f}); // 166
  pts.push_back({cx + s * 2.10f, cy + s * 0.30f, cz + s * 2.52f}); // 167

  // === LEFT EYE PATCH [168-171] (diamond, -Y side) ===
  pts.push_back(
      {cx + s * 2.38f, cy + s * (-0.28f), cz + s * 2.28f}); // 168: top
  pts.push_back(
      {cx + s * 2.42f, cy + s * (-0.38f), cz + s * 2.15f}); // 169: outer
  pts.push_back(
      {cx + s * 2.38f, cy + s * (-0.28f), cz + s * 2.00f}); // 170: bottom
  pts.push_back(
      {cx + s * 2.42f, cy + s * (-0.18f), cz + s * 2.15f}); // 171: inner

  // === RIGHT EYE PATCH [172-175] (diamond, +Y side) ===
  pts.push_back({cx + s * 2.38f, cy + s * 0.28f, cz + s * 2.28f}); // 172
  pts.push_back({cx + s * 2.42f, cy + s * 0.38f, cz + s * 2.15f}); // 173
  pts.push_back({cx + s * 2.38f, cy + s * 0.28f, cz + s * 2.00f}); // 174
  pts.push_back({cx + s * 2.42f, cy + s * 0.18f, cz + s * 2.15f}); // 175

  // === NOSE [176] ===
  pts.push_back({cx + s * 2.78f, cy, cz + s * 1.88f});

  // === TAIL [177-179] ===
  pts.push_back({cx + s * (-1.25f), cy, cz + s * 2.00f}); // 177: base
  pts.push_back(
      {cx + s * (-1.45f), cy + s * 0.05f, cz + s * 2.20f}); // 178: mid
  pts.push_back(
      {cx + s * (-1.55f), cy + s * (-0.05f), cz + s * 2.32f}); // 179: tip

  return pts; // 180 points
}

/**
 * makePandaEdges
 * Defines line segments between panda vertices.
 */
static std::vector<std::pair<int, int>> makePandaEdges() {
  std::vector<std::pair<int, int>> e;

  const int N = 8;
  const int NL = 6;

  auto ringE = [&](int s, int n) {
    for (int i = 0; i < n; i++)
      e.push_back({s + i, s + (i + 1) % n});
  };
  auto railE = [&](int s1, int s2, int n) {
    for (int i = 0; i < n; i++)
      e.push_back({s1 + i, s2 + i});
  };

  // ── BODY [edges 0-95] ──────────────────────────────────────────
  for (int r = 0; r < 6; r++)
    ringE(r * N, N);
  for (int r = 0; r < 5; r++)
    railE(r * N, (r + 1) * N, N);
  railE(40, 48, N); // neck bridge

  // ── HEAD [edges 96-167] ────────────────────────────────────────
  for (int r = 0; r < 5; r++)
    ringE(48 + r * N, N);
  for (int r = 0; r < 4; r++)
    railE(48 + r * N, 48 + (r + 1) * N, N);

  // ── LEGS [edges 168-287] ───────────────────────────────────────
  int legStart[] = {88, 106, 124, 142};
  for (int l = 0; l < 4; l++) {
    int ls = legStart[l];
    for (int r = 0; r < 3; r++)
      ringE(ls + r * NL, NL);
    for (int r = 0; r < 2; r++)
      railE(ls + r * NL, ls + (r + 1) * NL, NL);
  }

  // ── EARS [edges 288-295] ───────────────────────────────────────
  e.push_back({160, 161});
  e.push_back({161, 162});
  e.push_back({162, 163});
  e.push_back({163, 160});
  e.push_back({164, 165});
  e.push_back({165, 166});
  e.push_back({166, 167});
  e.push_back({167, 164});

  // ── EYE PATCHES [edges 296-303] ────────────────────────────────
  e.push_back({168, 169});
  e.push_back({169, 170});
  e.push_back({170, 171});
  e.push_back({171, 168});
  e.push_back({172, 173});
  e.push_back({173, 174});
  e.push_back({174, 175});
  e.push_back({175, 172});

  // ── NOSE → snout ring [edges 304-307] ──────────────────────────
  e.push_back({176, 80});
  e.push_back({176, 82});
  e.push_back({176, 84});
  e.push_back({176, 86});

  // ── TAIL [edges 308-311] ───────────────────────────────────────
  e.push_back({177, 178});
  e.push_back({178, 179});
  e.push_back({177, 0}); // tail base → rump top
  e.push_back({177, 4}); // tail base → rump bottom

  // ── EAR → HEAD connections [edges 312-315] ─────────────────────
  // Ring at 56 (back skull): i=0 is top(+Z), i=7 is top-left(-Y)
  //                          i=1 is top-right(+Y)
  // Ring at 64 (mid skull):  same layout
  e.push_back({163, 63}); // L ear base → back-skull top-left
  e.push_back({163, 71}); // L ear base → mid-skull top-left
  e.push_back({167, 57}); // R ear base → back-skull top-right
  e.push_back({167, 65}); // R ear base → mid-skull top-right

  return e; // 316 edges
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
    int thick = 2;

    if (type == ObjectType::BANANA) {
      if (i < 24) {
        color = cv::Scalar(255, 255, 0); // cyan slice diamonds
      } else if (i < 44) {
        color = cv::Scalar(0, 255, 255); // yellow rails
      } else {
        color = cv::Scalar(30, 60, 120); // stem/tip
      }
    } else if (type == ObjectType::PANDA) {
      if (i < 168) {
        // Body + head: bright white
        color = cv::Scalar(255, 255, 255);
      } else if (i < 288) {
        // Legs: charcoal
        color = cv::Scalar(70, 70, 80);
      } else if (i < 304) {
        // Ears + eye patches: near-black
        color = cv::Scalar(35, 35, 45);
        thick = 3;
      } else {
        // Nose, tail, ear-head connectors: muted warm gray
        color = cv::Scalar(160, 150, 170);
      }
    } else {
      color = cv::Scalar(0, 255, 0);
    }

    cv::line(frame, p1, p2, color, thick, cv::LINE_AA);
  }

  return 0;
}