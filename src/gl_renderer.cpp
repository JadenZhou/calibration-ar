/**
 * gl_renderer.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: OpenGL rendering utilities implementation.
 */

#include "gl_renderer.h"

#include <cstring>
#include <iostream>

GLFWwindow *initGL(int width, int height, const char *title) {
  if (!glfwInit()) {
    std::cerr << "Error: failed to initialize GLFW\n";
    return nullptr;
  }

  // Request a compatibility profile so legacy fixed-function GL works
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window) {
    std::cerr << "Error: failed to create GLFW window\n";
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    std::cerr << "Error: GLEW init failed: " << glewGetErrorString(err) << "\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return nullptr;
  }

  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  return window;
}

GLuint uploadCameraFrame(const cv::Mat &frame, GLuint texID) {
  if (frame.empty())
    return texID;

  // OpenCV is BGR, top-to-bottom.  OpenGL expects bottom-to-top.
  cv::Mat flipped;
  cv::flip(frame, flipped, 0);

  if (texID == 0) {
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, flipped.cols, flipped.rows, 0,
                 GL_BGR, GL_UNSIGNED_BYTE, flipped.data);
  } else {
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, flipped.cols, flipped.rows, GL_BGR,
                    GL_UNSIGNED_BYTE, flipped.data);
  }

  return texID;
}

void drawCameraBackground(GLuint texID, int width, int height) {
  // Save state
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);

  // Set up orthographic projection filling the whole viewport
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, width, 0, height, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glBindTexture(GL_TEXTURE_2D, texID);
  glColor3f(1.0f, 1.0f, 1.0f);

  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(0, 0);
  glTexCoord2f(1, 0);
  glVertex2f((float)width, 0);
  glTexCoord2f(1, 1);
  glVertex2f((float)width, (float)height);
  glTexCoord2f(0, 1);
  glVertex2f(0, (float)height);
  glEnd();

  // Restore state
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();

  // Clear only the depth buffer so 3D objects render on top
  glClear(GL_DEPTH_BUFFER_BIT);
}

void buildProjectionMatrix(const cv::Mat &cameraMatrix, int w, int h, double n,
                           double f, double proj[16]) {
  double fx = cameraMatrix.at<double>(0, 0);
  double fy = cameraMatrix.at<double>(1, 1);
  double cx = cameraMatrix.at<double>(0, 2);
  double cy = cameraMatrix.at<double>(1, 2);

  std::memset(proj, 0, 16 * sizeof(double));

  proj[0] = 2.0 * fx / w;
  proj[5] = 2.0 * fy / h;
  proj[8] = 1.0 - 2.0 * cx / w;
  proj[9] = 2.0 * cy / h - 1.0;
  proj[10] = -(f + n) / (f - n);
  proj[11] = -1.0;
  proj[14] = -2.0 * f * n / (f - n);
}

void buildModelViewMatrix(const cv::Mat &rvec, const cv::Mat &tvec,
                          double mv[16]) {
  cv::Mat R;
  cv::Rodrigues(rvec, R);

  // OpenCV: X-right, Y-down,  Z-away
  // OpenGL: X-right, Y-up,    Z-toward viewer
  // Flip rows 1 and 2 (negate Y and Z components)

  mv[0] = R.at<double>(0, 0);
  mv[1] = -R.at<double>(1, 0);
  mv[2] = -R.at<double>(2, 0);
  mv[3] = 0.0;

  mv[4] = R.at<double>(0, 1);
  mv[5] = -R.at<double>(1, 1);
  mv[6] = -R.at<double>(2, 1);
  mv[7] = 0.0;

  mv[8] = R.at<double>(0, 2);
  mv[9] = -R.at<double>(1, 2);
  mv[10] = -R.at<double>(2, 2);
  mv[11] = 0.0;

  mv[12] = tvec.at<double>(0, 0);
  mv[13] = -tvec.at<double>(1, 0);
  mv[14] = -tvec.at<double>(2, 0);
  mv[15] = 1.0;
}

void setupLighting() {
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_NORMALIZE); // auto-normalize normals after scaling

  // Light positioned above and in front of the board (in eye space)
  GLfloat pos[] = {0.0f, 0.0f, 1.0f, 0.0f}; // directional, from camera
  GLfloat ambient[] = {0.25f, 0.25f, 0.25f, 1.0f};
  GLfloat diffuse[] = {0.85f, 0.85f, 0.85f, 1.0f};
  GLfloat spec[] = {0.4f, 0.4f, 0.4f, 1.0f};

  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
}

void cleanupGL(GLFWwindow *window) {
  if (window)
    glfwDestroyWindow(window);
  glfwTerminate();
}