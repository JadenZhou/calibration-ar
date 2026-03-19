/**
 * image_io.cpp
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Utilities for writing saved calibration images to disk.
 */

#include "image_io.h"

#include <iomanip>
#include <iostream>
#include <sstream>

int writeSavedImages(const std::string &dir,
                     const std::vector<cv::Mat> &image_list) {
  for (size_t i = 0; i < image_list.size(); i++) {
    if (image_list[i].empty()) {
      std::cerr << "Error: saved image " << i << " is empty\n";
      return -1;
    }

    std::ostringstream name;
    name << dir << "/calib_" << std::setw(3) << std::setfill('0') << i
         << ".png";

    if (!cv::imwrite(name.str(), image_list[i])) {
      std::cerr << "Error: failed to write " << name.str() << "\n";
      return -2;
    }
  }

  return 0;
}

int saveImage(const std::string &dir, const std::string &prefix,
              const cv::Mat &frame) {
  static int img_idx = 0;

  if (frame.empty()) {
    std::cerr << "Error: cannot save empty image\n";
    return -1;
  }

  std::ostringstream filename;
  filename << dir << "/" << prefix << "_" << std::setw(3) << std::setfill('0')
           << img_idx++ << ".png";

  if (!cv::imwrite(filename.str(), frame)) {
    std::cerr << "Error: failed to write " << filename.str() << "\n";
    return -2;
  }

  std::cout << "Saved image: " << filename.str() << "\n";
  return 0;
}