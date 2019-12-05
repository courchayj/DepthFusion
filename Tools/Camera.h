/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/

#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"

#define GLM_ENABLE_EXPERIMENTAL

#include "Variables.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

namespace PixMeshFusion {

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;
/*********************************************************************************************************/
class camera {
public:
    camera() {}
    camera(float fx,float fy, float skew, float ux, float uy, int width, int height);
    void create(float fx,float fy, float skew, float ux, float uy, int width, int height);
    void computeOpenGLCamera();
    mat4 getOpenGLCamera() {return intrinsics_gl; }
    mat3 getOpenGLCameraFull() {return intrinsics_gl_full; }
    mat3 getInvertOpenGLCamera() {return intrinsics_inv_glm; }
    inline float getFocal() {return intrinsics(0,0);}
private:
    matrix<3,3> intrinsics;
    mat4        intrinsics_gl;
    mat3        intrinsics_gl_full;
    mat3        intrinsics_glm_cpu;
    mat3        intrinsics_inv_glm;
    int w;
    int h;
};
/*********************************************************************************************************/
}
#endif // CAMERA_H
