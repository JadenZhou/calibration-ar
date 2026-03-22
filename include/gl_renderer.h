/**
 * gl_renderer.h
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: OpenGL rendering utilities for AR overlay.
 */

#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include <opencv2/opencv.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/**
 * initGL
 * Creates a GLFW window with an OpenGL context and initializes GLEW.
 *
 * Arguments:
 * - width: window width in pixels
 * - height: window height in pixels
 * - title: window title
 *
 * Returns:
 * - pointer to GLFW window, or nullptr on failure
 */
GLFWwindow *initGL(int width, int height, const char *title);

/**
 * uploadCameraFrame
 * Uploads a cv::Mat (BGR, 8UC3) to an OpenGL texture.
 * Creates a new texture if texID is 0; updates in-place otherwise.
 *
 * Arguments:
 * - frame: camera frame (BGR)
 * - texID: existing texture ID, or 0 to create new
 *
 * Returns:
 * - OpenGL texture ID
 */
GLuint uploadCameraFrame(const cv::Mat &frame, GLuint texID = 0);

/**
 * drawCameraBackground
 * Renders the camera texture as a fullscreen quad behind everything else.
 *
 * Arguments:
 * - texID: camera texture
 * - width: viewport width
 * - height: viewport height
 */
void drawCameraBackground(GLuint texID, int width, int height);

/**
 * buildProjectionMatrix
 * Converts OpenCV camera intrinsics into an OpenGL-compatible
 * 4x4 column-major projection matrix.
 *
 * Arguments:
 * - cameraMatrix: 3x3 CV_64F intrinsic matrix
 * - w: image width
 * - h: image height
 * - n: near clipping plane
 * - f: far clipping plane
 * - proj: output 16-element column-major array
 */
void buildProjectionMatrix(const cv::Mat &cameraMatrix, int w, int h, double n,
                           double f, double proj[16]);

/**
 * buildModelViewMatrix
 * Converts solvePnP rvec/tvec into an OpenGL-compatible
 * 4x4 column-major modelview matrix.
 * Handles the OpenCV (Y-down, Z-away) to OpenGL (Y-up, Z-toward) flip.
 *
 * Arguments:
 * - rvec: rotation vector from solvePnP
 * - tvec: translation vector from solvePnP
 * - mv: output 16-element column-major array
 */
void buildModelViewMatrix(const cv::Mat &rvec, const cv::Mat &tvec,
                          double mv[16]);

/**
 * setupLighting
 * Configures a basic OpenGL light for shaded rendering.
 */
void setupLighting();

/**
 * cleanupGL
 * Destroys GLFW window and terminates GLFW.
 */
void cleanupGL(GLFWwindow *window);

#endif