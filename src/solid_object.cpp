/**
 * solid_object.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Solid 3D object utilities implementation.
 */

#include "solid_object.h"
#include "gl_teapot.h"

#include <GL/glew.h>

#include <algorithm>
#include <cmath>
#include <iostream>

// ── Enum helpers ─────────────────────────────────────────────

SolidType nextSolidType(SolidType type) {
  switch (type) {
  case SolidType::HOUSE:
    return SolidType::TEAPOT;
  case SolidType::TEAPOT:
    return SolidType::HOUSE;
  default:
    return SolidType::HOUSE;
  }
}

const char *solidTypeName(SolidType type) {
  switch (type) {
  case SolidType::HOUSE:
    return "House";
  case SolidType::TEAPOT:
    return "Teapot";
  default:
    return "Unknown";
  }
}

// ── House geometry ───────────────────────────────────────────

std::vector<cv::Point3f> makeSolidHousePoints() {
  const float s = 2.0f;
  const float cz = 2.0f;

  return {{s * 0.0f, s * 0.0f, cz + s * 0.0f},
          {s * 2.0f, s * 0.0f, cz + s * 0.0f},
          {s * 2.0f, s * -2.0f, cz + s * 0.0f},
          {s * 0.0f, s * -2.0f, cz + s * 0.0f},

          {s * 0.0f, s * 0.0f, cz + s * 2.0f},
          {s * 2.0f, s * 0.0f, cz + s * 2.0f},
          {s * 2.0f, s * -2.0f, cz + s * 2.0f},
          {s * 0.0f, s * -2.0f, cz + s * 2.0f},

          {s * 1.0f, s * 0.0f, cz + s * 3.0f},
          {s * 1.0f, s * -2.0f, cz + s * 3.0f}};
}

std::vector<std::vector<int>> makeSolidHouseFaces() {
  return {{0, 1, 2, 3}, {0, 1, 5, 4}, {1, 2, 6, 5}, {2, 3, 7, 6}, {3, 0, 4, 7},
          {4, 5, 8},    {7, 6, 9},    {4, 8, 9, 7}, {5, 6, 9, 8}};
}

// ── Face normal helper ───────────────────────────────────────

static cv::Point3f computeFaceNormal(const std::vector<cv::Point3f> &pts,
                                     const std::vector<int> &face) {
  cv::Point3f a = pts[face[0]];
  cv::Point3f b = pts[face[1]];
  cv::Point3f c = pts[face[2]];

  cv::Point3f u = b - a;
  cv::Point3f v = c - a;

  cv::Point3f n(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x);

  float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
  if (len > 1e-6f) {
    n.x /= len;
    n.y /= len;
    n.z /= len;
  }
  return n;
}

// ── GL renderers (per type) ──────────────────────────────────

static void renderHouseGL() {
  auto pts = makeSolidHousePoints();
  auto faces = makeSolidHouseFaces();

  float colors[][3] = {
      {0.706f, 0.706f, 0.706f}, {1.000f, 0.667f, 0.314f},
      {0.471f, 0.863f, 0.314f}, {0.314f, 0.667f, 1.000f},
      {0.863f, 0.471f, 0.863f}, {0.863f, 0.314f, 0.314f},
      {0.784f, 0.314f, 0.314f}, {1.000f, 0.471f, 0.471f},
      {0.922f, 0.471f, 0.471f},
  };

  for (size_t i = 0; i < faces.size(); i++) {
    cv::Point3f n = computeFaceNormal(pts, faces[i]);
    glColor3fv(colors[i % 9]);
    glNormal3f(n.x, n.y, n.z);

    glBegin(GL_POLYGON);
    for (int idx : faces[i]) {
      glVertex3f(pts[idx].x, pts[idx].y, pts[idx].z);
    }
    glEnd();
  }

  // Black wireframe edges for definition
  glDisable(GL_LIGHTING);
  glColor3f(0.0f, 0.0f, 0.0f);
  glLineWidth(2.0f);
  for (const auto &face : faces) {
    glBegin(GL_LINE_LOOP);
    for (int idx : face) {
      glVertex3f(pts[idx].x, pts[idx].y, pts[idx].z);
    }
    glEnd();
  }
  glEnable(GL_LIGHTING);
}

static void renderTeapot() {
  glPushMatrix();

  // Position on board: center of the checkerboard area
  glTranslatef(4.0f, -2.5f, 0.0f);

  float teapotScale = 2.0f;
  glScalef(teapotScale, teapotScale, teapotScale);

  // Terracotta ceramic color
  glColor3f(0.82f, 0.41f, 0.12f);

  // Specular highlight for glazed look
  GLfloat spec[] = {0.9f, 0.9f, 0.9f, 1.0f};
  GLfloat shin[] = {60.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shin);

  renderTeapotGL(1.0f, 16);

  // Reset specular
  GLfloat nospec[] = {0.0f, 0.0f, 0.0f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, nospec);

  glPopMatrix();
}

// ── Public dispatch ──────────────────────────────────────────

void renderSolidGL(SolidType type) {
  switch (type) {
  case SolidType::HOUSE:
    renderHouseGL();
    break;
  case SolidType::TEAPOT:
    renderTeapot();
    break;
  }
}

// ── Legacy OpenCV rendering (unchanged) ──────────────────────

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

  for (size_t i = 0; i < faces.size(); i++) {
    double avgz = 0.0;
    for (int idx : faces[i])
      avgz += camPts[idx].z;
    avgz /= faces[i].size();
    ordered.push_back({faces[i], avgz, palette[i % palette.size()]});
  }

  std::sort(
      ordered.begin(), ordered.end(),
      [](const FaceInfo &a, const FaceInfo &b) { return a.depth > b.depth; });

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