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

# OpenGL / GLEW / GLFW (macOS)
GL_CFLAGS = $(shell pkg-config --cflags glew glfw3)
GL_LIBS   = $(shell pkg-config --libs glew glfw3) -framework OpenGL

CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I$(INCDIR) $(OPENCV_CFLAGS)
LDLIBS   = $(OPENCV_LIBS)

# Combined flags for GL targets
CXXFLAGS_GL = $(CXXFLAGS) $(GL_CFLAGS)
LDLIBS_GL   = $(LDLIBS) $(GL_LIBS)

.PHONY: all clean calibrate pose features dirs

all: calibrate pose features

dirs:
	mkdir -p $(BINDIR) $(OBJDIR)

# Generic compile rule (non-GL sources)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | dirs
	$(CXX) $(CXXFLAGS) -c $< -o $@

# GL-specific compile rule for files that #include GL headers
$(OBJDIR)/gl_renderer.o: $(SRCDIR)/gl_renderer.cpp | dirs
	$(CXX) $(CXXFLAGS_GL) -c $< -o $@

$(OBJDIR)/pose.o: $(SRCDIR)/pose.cpp | dirs
	$(CXX) $(CXXFLAGS_GL) -c $< -o $@

$(OBJDIR)/solid_object.o: $(SRCDIR)/solid_object.cpp | dirs
	$(CXX) $(CXXFLAGS_GL) -c $< -o $@

# --- Object files ---

# Shared
COMMON_OBJS = $(OBJDIR)/checkerboard.o

# Calibration
CALIB_OBJS = \
	$(OBJDIR)/calibration.o \
	$(OBJDIR)/calibration_utils.o \
	$(OBJDIR)/intrinsics.o \
	$(OBJDIR)/image_io.o \
	$(COMMON_OBJS)

# Pose + AR (now includes gl_renderer)
POSE_OBJS = \
	$(OBJDIR)/pose.o \
	$(OBJDIR)/pose_utils.o \
	$(OBJDIR)/calibration_utils.o \
	$(OBJDIR)/image_io.o \
	$(OBJDIR)/ar_object.o \
	$(OBJDIR)/solid_object.o \
	$(OBJDIR)/gl_renderer.o \
	$(COMMON_OBJS)

# Feature detection
FEAT_OBJS = \
	$(OBJDIR)/features.o \
	$(OBJDIR)/features_utils.o \
	$(OBJDIR)/image_io.o

# --- Executables ---

calibrate: $(CALIB_OBJS) | dirs
	$(CXX) $^ -o $(BINDIR)/$@ $(LDLIBS)

pose: $(POSE_OBJS) | dirs
	$(CXX) $^ -o $(BINDIR)/$@ $(LDLIBS_GL)

features: $(FEAT_OBJS) | dirs
	$(CXX) $^ -o $(BINDIR)/$@ $(LDLIBS)

# --- Clean ---

clean:
	rm -rf $(OBJDIR)
	rm -f $(BINDIR)/calibrate $(BINDIR)/pose $(BINDIR)/features
	rm -f *~ $(SRCDIR)/*~ $(INCDIR)/*~