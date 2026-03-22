/**
 * pose.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Live pose estimation with OpenGL-shaded AR objects.
 */

#include <opencv2/opencv.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <iostream>
#include <vector>

#include "ar_object.h"
#include "calibration.h"
#include "checkerboard.h"
#include "gl_renderer.h"
#include "gl_teapot.h"
#include "image_io.h"
#include "pose.h"
#include "solid_object.h"

// Keyboard state (GLFW delivers events, not polled chars like cv::waitKey)
static bool keyPressed[256] = {false};

static void keyCallback(GLFWwindow *window, int key, int /*scancode*/,
                        int action, int /*mods*/) {
  if (key >= 0 && key < 256) {
    if (action == GLFW_PRESS) {
      keyPressed[key] = true;
    }
  }
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

static ObjectType nextObjectType(ObjectType type) {
  switch (type) {
  case ObjectType::BANANA:
    return ObjectType::PANDA;
  case ObjectType::PANDA:
    return ObjectType::BANANA;
  default:
    return ObjectType::BANANA;
  }
}

static const char *objectTypeName(ObjectType type) {
  switch (type) {
  case ObjectType::BANANA:
    return "Banana";
  case ObjectType::PANDA:
    return "Panda";
  default:
    return "Unknown";
  }
}

// Helper: read a frame from GL framebuffer into cv::Mat for screenshots
static cv::Mat readFramebuffer(int w, int h) {
  cv::Mat img(h, w, CV_8UC3);
  glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, img.data);
  cv::flip(img, img, 0); // OpenGL is bottom-up
  return img;
}

int main() {
  // ── Load intrinsics ────────────────────────────────────────
  cv::Mat cameraMatrix, distCoeffs;
  int rc = readIntrinsics("data/calibration.yml", cameraMatrix, distCoeffs);
  if (rc != 0) {
    std::cerr << "Error: failed to load intrinsics\n";
    return -1;
  }

  // ── Open camera ────────────────────────────────────────────
  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    std::cerr << "Error: could not open camera\n";
    return -1;
  }

  // Grab one frame to get resolution
  cv::Mat frame;
  cap >> frame;
  if (frame.empty()) {
    std::cerr << "Error: empty initial frame\n";
    return -1;
  }
  int W = frame.cols;
  int H = frame.rows;

  // ── Init OpenGL ────────────────────────────────────────────
  GLFWwindow *window = initGL(W, H, "Pose + AR (OpenGL)");
  if (!window)
    return -1;

  glfwSetKeyCallback(window, keyCallback);

  GLuint camTex = 0; // camera background texture

  // ── Build GL projection matrix from intrinsics ─────────────
  double glProj[16];
  buildProjectionMatrix(cameraMatrix, W, H, 0.1, 1000.0, glProj);

  // ── AR state ───────────────────────────────────────────────
  cv::Size patternSize(9, 6);
  std::vector<cv::Vec3f> point_set = buildPointSet(patternSize);

  ObjectType currentObject = ObjectType::BANANA;
  auto objectPoints = makePoints(currentObject);
  auto objectEdges = makeEdges(currentObject);

  bool showAxes = true;
  bool showWireframe = true;
  SolidType currentSolid = SolidType::HOUSE;
  bool showSolid = true;

  std::vector<cv::Point2f> corners;
  cv::Mat rvec, tvec;

  // ── Main loop ──────────────────────────────────────────────
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // ── Handle key presses ─────────────────────────────────
    if (keyPressed['Q'] || keyPressed['q'])
      break;

    if (keyPressed['A'] || keyPressed['a']) {
      showAxes = !showAxes;
      keyPressed['A'] = keyPressed['a'] = false;
    }
    if (keyPressed['W'] || keyPressed['w']) {
      showWireframe = !showWireframe;
      keyPressed['W'] = keyPressed['w'] = false;
    }
    if (keyPressed['E'] || keyPressed['e']) {
      currentObject = nextObjectType(currentObject);
      objectPoints = makePoints(currentObject);
      objectEdges = makeEdges(currentObject);
      std::cout << "Switched to " << objectTypeName(currentObject) << "\n";
      keyPressed['E'] = keyPressed['e'] = false;
    }
    if (keyPressed['F'] || keyPressed['f']) {
      showSolid = !showSolid;
      keyPressed['F'] = keyPressed['f'] = false;
    }
    if (keyPressed['R'] || keyPressed['r']) {
      currentSolid = nextSolidType(currentSolid);
      std::cout << "Switched solid to " << solidTypeName(currentSolid) << "\n";
      keyPressed['R'] = keyPressed['r'] = false;
    }
    if (keyPressed['P'] || keyPressed['p']) {
      cv::Mat screenshot = readFramebuffer(W, H);
      saveImage("out/images", "pose_gl", screenshot);
      keyPressed['P'] = keyPressed['p'] = false;
    }

    // ── Grab camera frame ──────────────────────────────────
    cap >> frame;
    if (frame.empty())
      break;

    // ── Detect checkerboard & estimate pose ────────────────
    bool found = detectCheckerboard(frame, patternSize, corners);
    bool poseOK = false;

    if (found) {
      rc = estimatePose(point_set, corners, cameraMatrix, distCoeffs, rvec,
                        tvec);
      poseOK = (rc == 0);
    }

    // ── Draw HUD text onto the camera frame (OpenCV) ───────
    if (found && poseOK) {
      drawCorners(frame, patternSize, corners, found);

      cv::putText(frame, "Pose found", cv::Point(20, 30),
                  cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);

      char buf[256];
      std::snprintf(buf, sizeof(buf), "tvec: [%.2f, %.2f, %.2f]",
                    tvec.at<double>(0), tvec.at<double>(1), tvec.at<double>(2));
      cv::putText(frame, buf, cv::Point(20, 65), cv::FONT_HERSHEY_SIMPLEX, 0.7,
                  cv::Scalar(255, 255, 0), 2);

      std::snprintf(buf, sizeof(buf), "Wire: %s | Solid: %s",
                    objectTypeName(currentObject), solidTypeName(currentSolid));
      cv::putText(frame, buf, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 0.7,
                  cv::Scalar(255, 200, 0), 2);
      cv::putText(frame, buf, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 0.7,
                  cv::Scalar(255, 200, 0), 2);
    } else {
      cv::putText(frame, found ? "solvePnP failed" : "Checkerboard not found",
                  cv::Point(20, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8,
                  cv::Scalar(0, 0, 255), 2);
    }

    cv::putText(frame,
                "[a] axes [w] wire [e] next wire [f] solid [r] next solid "
                "[p] screenshot [q] quit",
                cv::Point(20, frame.rows - 20), cv::FONT_HERSHEY_SIMPLEX, 0.45,
                cv::Scalar(255, 255, 255), 1);

    // If pose is good, also draw wireframe + axes with OpenCV
    // (these are 2D overlays baked into the camera texture)
    if (poseOK) {
      if (showAxes) {
        std::vector<cv::Point2f> axisImg;
        if (projectAxes(10.0f, rvec, tvec, cameraMatrix, distCoeffs, axisImg) ==
            0) {
          drawAxes(frame, axisImg);
        }
      }
      if (showWireframe) {
        std::vector<cv::Point2f> objImg;
        if (projectObject(objectPoints, rvec, tvec, cameraMatrix, distCoeffs,
                          objImg) == 0) {
          drawObject(frame, currentObject, objImg, objectEdges);
        }
      }
    }

    // ── OpenGL rendering ───────────────────────────────────
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, W, H);

    // 1) Camera background (includes wireframe/axes/HUD baked in)
    camTex = uploadCameraFrame(frame, camTex);
    drawCameraBackground(camTex, W, H);

    // 2) GL-shaded solid object (rendered in 3D on top)
    if (poseOK && showSolid) {
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixd(glProj);

      double mv[16];
      buildModelViewMatrix(rvec, tvec, mv);
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixd(mv);

      setupLighting();

      // --- Solid house (your existing object) ---
      renderSolidGL(currentSolid);

      // Reset specular so it doesn't bleed into other objects
      GLfloat nospec[] = {0.0f, 0.0f, 0.0f, 1.0f};
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, nospec);

      glPopMatrix();

      glDisable(GL_LIGHTING);
    }

    glfwSwapBuffers(window);
  }

  // ── Cleanup ────────────────────────────────────────────────
  if (camTex)
    glDeleteTextures(1, &camTex);
  cap.release();
  cleanupGL(window);

  return 0;
}