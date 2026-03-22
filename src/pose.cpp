/**
 * pose.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Live pose estimation, projected axes, and virtual object.
 */

#include <opencv2/opencv.hpp>

#include <cstdio>
#include <iostream>
#include <vector>

#include "ar_object.h"
#include "calibration.h"
#include "checkerboard.h"
#include "image_io.h"
#include "pose.h"
#include "solid_object.h"

static void printPose(const cv::Mat &rvec, const cv::Mat &tvec) {
  std::cout << "rvec: [" << rvec.at<double>(0, 0) << ", "
            << rvec.at<double>(1, 0) << ", " << rvec.at<double>(2, 0) << "]   ";

  std::cout << "tvec: [" << tvec.at<double>(0, 0) << ", "
            << tvec.at<double>(1, 0) << ", " << tvec.at<double>(2, 0) << "]\n";
}

int main() {
  cv::Mat cameraMatrix, distCoeffs;
  int rc = readIntrinsics("data/calibration.yml", cameraMatrix, distCoeffs);

  if (rc != 0) {
    std::cerr << "Error: failed to load intrinsics\n";
    return -1;
  }

  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    std::cerr << "Error: could not open camera\n";
    return -1;
  }

  cv::Size patternSize(9, 6);
  std::vector<cv::Vec3f> point_set = buildPointSet(patternSize);

  ObjectType currentObject = ObjectType::BANANA;

  auto objectPoints = makePoints(currentObject);
  auto objectEdges = makeEdges(currentObject);

  std::vector<cv::Point3f> solidPoints = makeSolidHousePoints();
  std::vector<std::vector<int>> solidFaces = makeSolidHouseFaces();
  std::vector<cv::Point2f> solidImagePoints;
  bool showSolid = true;

  cv::Mat frame;
  std::vector<cv::Point2f> corners;
  cv::Mat rvec, tvec;
  std::vector<cv::Point2f> axisImagePoints;
  std::vector<cv::Point2f> objectImagePoints;

  bool showAxes = true;
  bool showObject = true;

  while (true) {
    cap >> frame;
    if (frame.empty()) {
      std::cerr << "Error: empty frame\n";
      break;
    }

    bool found = detectCheckerboard(frame, patternSize, corners);

    if (found) {
      drawCorners(frame, patternSize, corners, found);

      rc = estimatePose(point_set, corners, cameraMatrix, distCoeffs, rvec,
                        tvec);

      if (rc == 0) {
        printPose(rvec, tvec);

        if (showAxes) {
          rc = projectAxes(10.0f, rvec, tvec, cameraMatrix, distCoeffs,
                           axisImagePoints);
          if (rc == 0) {
            drawAxes(frame, axisImagePoints);
          }
        }

        if (showObject) {
          rc = projectObject(objectPoints, rvec, tvec, cameraMatrix, distCoeffs,
                             objectImagePoints);
          if (rc == 0) {
            drawObject(frame, currentObject, objectImagePoints, objectEdges);
          }
        }

        if (showSolid) {
          rc = projectSolidObject(solidPoints, rvec, tvec, cameraMatrix,
                                  distCoeffs, solidImagePoints);
          if (rc == 0) {
            drawSolidObject(frame, solidPoints, solidImagePoints, solidFaces,
                            rvec, tvec);
          }
        }

        cv::putText(frame, "Pose found", cv::Point(20, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);

        char pose_buf[256];
        std::snprintf(pose_buf, sizeof(pose_buf), "tvec: [%.2f, %.2f, %.2f]",
                      tvec.at<double>(0, 0), tvec.at<double>(1, 0),
                      tvec.at<double>(2, 0));

        cv::putText(frame, pose_buf, cv::Point(20, 65),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 0), 2);
      } else {
        cv::putText(frame, "solvePnP failed", cv::Point(20, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
      }
    } else {
      cv::putText(frame, "Checkerboard not found", cv::Point(20, 30),
                  cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
    }

    cv::putText(frame,
                "[a] axes  [o] wireframe  [f] solid  [p] screenshot  [q] quit",
                cv::Point(20, frame.rows - 20), cv::FONT_HERSHEY_SIMPLEX, 0.55,
                cv::Scalar(255, 255, 255), 1);

    cv::imshow("Pose + AR Object", frame);

    char key = (char)cv::waitKey(1);
    if (key == 'q') {
      break;
    } else if (key == 'a') {
      showAxes = !showAxes;
    } else if (key == 'o') {
      showObject = !showObject;
    } else if (key == 'p') {
      int save_rc = saveImage("out/images", "pose", frame);
      if (save_rc != 0) {
        std::cout << "Failed to save image\n";
      }
    } else if (key == 'f') {
      showSolid = !showSolid;
    }
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}