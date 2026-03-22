# Calibration and AR

This project is a C++ application suite for real-time camera calibration, 3D pose estimation, and Augmented Reality (AR) visualization using OpenCV and OpenGL. It consists of three main modules: a calibration tool, a pose estimation/AR renderer, and a feature detection demo.

---

## Features

* **Camera Calibration:** Detects checkerboard patterns to calculate intrinsic camera parameters and distortion coefficients.
* **3D Pose Estimation:** Uses calibrated camera data to determine the real-time position and orientation of a checkerboard target.
* **AR Overlay:**
    * **Wireframe Objects:** Renders custom 3D wireframes (Banana and Panda) using OpenCV drawing functions.
    * **Solid Objects:** Renders OpenGL-shaded 3D objects (House and Utah Teapot) with real-time lighting and depth testing.
* **ORB Feature Detection:** A live demo for detecting and visualizing ORB keypoints with adjustable sensitivity.

---

## Dependencies

* **Compiler:** `clang++` or `g++` (C++17 support required)
* **Libraries:**
    * OpenCV 4.x
    * OpenGL
    * GLEW
    * GLFW3
* **Tools:** `pkg-config` (for library management)

---

## Building the Project

The project uses a `makefile` for compilation. Ensure you have the required dependencies installed via a package manager like Homebrew (macOS) or APT (Linux).

1.  **Initialize Directories:**
    ```bash
    make dirs
    ```
2.  **Compile All Executables:**
    ```bash
    make all
    ```
    *This creates `calibrate`, `pose`, and `features` in the `bin/` directory.*

3.  **Clean Build Files:**
    ```bash
    make clean
    ```

---

## Modules and Controls

### 1. Camera Calibration (`bin/calibrate`)
Used to calibrate your webcam. Requires a 9x6 internal corner checkerboard.
* **`s`**: Save current checkerboard detection frame.
* **`c`**: Run calibration (requires at least 5 saved frames).
* **`w`**: Write calibrated intrinsics to `data/calibration.yml`.
* **`i`**: Save all captured calibration images to `data/images/`.
* **`q`**: Quit.

### 2. Pose & AR Overlay (`bin/pose`)
The main AR application. It requires `data/calibration.yml` to be present.
* **`a`**: Toggle 3D coordinate axes overlay.
* **`w`**: Toggle wireframe object visibility.
* **`e`**: Cycle through wireframe objects (**Banana** or **Panda**).
* **`f`**: Toggle solid (OpenGL) object visibility.
* **`r`**: Cycle through solid objects (**House** or **Teapot**).
* **`p`**: Take a screenshot (saved to `out/images/`).
* **`q` / `ESC`**: Quit.

### 3. ORB Features (`bin/features`)
A demo of live feature detection.
* **`=` / `-`**: Increase/decrease maximum number of features.
* **`t` / `g`**: Increase/decrease FAST threshold.
* **`p`**: Take a screenshot.
* **`q`**: Quit.

---

## Project Structure

* `include/`: Header files for calibration, pose estimation, and rendering utilities.
* `src/`: 
    * `ar_object.cpp`: 3D point and edge definitions for the Banana and Panda.
    * `gl_renderer.cpp`: OpenGL context management and matrix conversion (OpenCV to OpenGL).
    * `gl_teapot.cpp`: Geometry data and tessellation for the Utah Teapot.
    * `solid_object.cpp`: Implementations for the 3D House and GL dispatch.
    * `pose.cpp`: The main entry point for the AR application.
* `data/`: Storage for `calibration.yml` and captured images.
* `out/`: Output directory for screenshots.
* `build/`: Temporary object files.
* `bin/`: Compiled executable binaries.