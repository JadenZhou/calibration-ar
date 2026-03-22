/**
 * gl_teapot.h
 * Name: Jaden Zhou
 * Date: Mar 2026
 * Purpose: Self-contained Utah teapot renderer using classic
 *          Bezier patch data (no GLUT dependency).
 */

#ifndef GL_TEAPOT_H
#define GL_TEAPOT_H

/**
 * renderTeapotGL
 * Renders the Utah teapot centered at the origin using OpenGL
 * fixed-function pipeline with normals for proper lighting.
 *
 * Arguments:
 * - size: overall scale factor
 * - gridN: tessellation resolution per patch (default 10)
 */
void renderTeapotGL(float size = 1.0f, int gridN = 10);

#endif