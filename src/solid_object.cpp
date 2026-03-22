/**
 * solid_object.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Solid 3D object utilities implementation.
 */

#include "solid_object.h"

#include <GL/glew.h>
#include <algorithm>
#include <iostream>

/**
 * makeSolidHousePoints
 * Builds a simple 3D house in world coordinates.
 */
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

cv::Point3f computeFaceNormal(const std::vector<cv::Point3f> &pts,
                              const std::vector<int> &face) {
  // Use first three vertices of the face
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

void renderSolidHouseGL() {
  auto pts = makeSolidHousePoints();
  auto faces = makeSolidHouseFaces();

  // Face colors: same palette as your OpenCV version but as 0-1 floats (RGB)
  // Original palette is BGR in OpenCV, convert to RGB floats
  float colors[][3] = {
      {0.706f, 0.706f, 0.706f}, // gray         walls
      {1.000f, 0.667f, 0.314f}, // orange
      {0.471f, 0.863f, 0.314f}, // green
      {0.314f, 0.667f, 1.000f}, // blue-orange
      {0.863f, 0.471f, 0.863f}, // pink
      {0.863f, 0.314f, 0.314f}, // red           roof sides
      {0.784f, 0.314f, 0.314f}, {1.000f, 0.471f, 0.471f}, // roof panels
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

  // Draw black wireframe edges on top for definition
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