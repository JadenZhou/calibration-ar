# makefile
# Name: Jaden Zhou
# Purpose: Build OpenCV programs for Calibration and AR.

CXX = clang++

# Directories
SRCDIR = src
INCDIR = include
BINDIR = bin
OBJDIR = build

# OpenCV
OPENCV_CFLAGS = $(shell pkg-config --cflags opencv4)
OPENCV_LIBS   = $(shell pkg-config --libs opencv4)

CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I$(INCDIR) $(OPENCV_CFLAGS)
LDLIBS   = $(OPENCV_LIBS)

.PHONY: all clean calibrate pose features dirs

all: calibrate pose features

dirs:
	mkdir -p $(BINDIR) $(OBJDIR)

# Generic compile rule
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | dirs
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Object files ---

# Shared
COMMON_OBJS = $(OBJDIR)/checkerboard.o

# Calibration
CALIB_OBJS = $(OBJDIR)/calibration.o $(OBJDIR)/calibration_utils.o $(OBJDIR)/intrinsics.o $(OBJDIR)/image_io.o $(COMMON_OBJS)

# Pose + AR
POSE_OBJS = $(OBJDIR)/pose.o $(COMMON_OBJS)

# Feature detection
FEAT_OBJS = $(OBJDIR)/features.o

# --- Executables ---

calibrate: $(CALIB_OBJS) | dirs
	$(CXX) $^ -o $(BINDIR)/$@ $(LDLIBS)

pose: $(POSE_OBJS) | dirs
	$(CXX) $^ -o $(BINDIR)/$@ $(LDLIBS)

features: $(FEAT_OBJS) | dirs
	$(CXX) $^ -o $(BINDIR)/$@ $(LDLIBS)

# --- Clean ---

clean:
	rm -rf $(OBJDIR)
	rm -f $(BINDIR)/calibrate $(BINDIR)/pose $(BINDIR)/features
	rm -f *~ $(SRCDIR)/*~ $(INCDIR)/*~