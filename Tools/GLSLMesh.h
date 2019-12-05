/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/

#ifndef GLSLMESHPROGRAM_H
#define GLSLMESHPROGRAM_H

#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <Tools/InterfaceOpenNI.h>
#include <Tools/Camera.h>
#include "Variables.h"

namespace PixMeshFusion {
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

/*********************************************************************************************************/
class GLSLMesh {
public:

    GLSLMesh() {allocated=false;glEnable(GL_CULL_FACE); glEnable(GL_DEPTH_TEST);}
    GLSLMesh(int width, int height, int scale_divide=1);
    ~GLSLMesh();
    inline int getWidth() {return w;}
    inline int getHeight() {return h;}
    inline float getFocal() {return intrinsics.getFocal();}
    void create(int width, int height,int scale_divide=1);
    void loadCamera(float fx,float fy, float skew, float ux, float uy,int width, int height);
    void attachReprojectionProgramColorToInt(GLuint handle, int order);
    void attachReprojectionProgramDepthToInt(GLuint handle, int order);
    void attachReprojectionProgramEnergyToInt(GLuint handle, int order);

    void setUniformInvertIntrinsics(GLuint handle);
    void setUniformIntrinsics(GLuint handle);
    void setUniformIntrinsicsFull(GLuint handle);
    void setUniformRotation(GLuint handle,float angle, float vecx, float vecy, float vecz);
    void setUniformRotation(GLuint handle,mat4 RT);
    void setUniformTranslation(GLuint handle,float vecx, float vecy, float vecz);
    void setUniformInt(GLuint handle,int iter);


    float* getVboData(int& size);

    void bindMesh();
    void bindEnergy();
    void bindMeshForComputeShader(int binding);
    void bindEnergy123ForComputeShader(int binding);
    void bindEnergy456ForComputeShader(int binding);
    inline void bindFrameBuffer(){glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);}
    inline void bindFrameBufferEnergy(){glBindFramebuffer(GL_FRAMEBUFFER, fboHandleEnergy);}
    inline void unbindFrameBuffer(){glBindFramebuffer(GL_FRAMEBUFFER, 0);}
    void free();
    void drawMesh(bool display);
    inline void setCameraIndex(int cam) {cam_index = cam;}
    void attachOpenGLCamera(mat4 intrinsics_glf) ;
    virtual void computeMeshFromDepth() {}
    static int      count_texture;
    int             current_texture;
private:    
    bool            allocated;
    float*          vertices;
    float*          uv;
    unsigned int*   indices;
    int             cam_index;
    GLuint          vao;
    GLuint          vbo;//vertex buffer object
    GLuint          ebo123;//energy buffer object
    GLuint          ebo456;//energy buffer object
    GLuint          ibo;//index buffer object
    GLuint          tbo;//energy buffer TO BE DELETED

    mat4            intrinsics_gl;
    mat3            intrinsics_gl_full;
    mat3            intrinsics_inv_glm;
    camera          intrinsics;

    int             scale_divide;
    GLuint          fboHandle;
    GLuint          fboHandleEnergy;
    GLuint          depthrenderbuffer;
    GLuint          depthrenderbufferEnergy;

    int w,h;
};
}
/*********************************************************************************************************/

#endif // GLSLMESHPROGRAM_H
