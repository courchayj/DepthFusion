/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/
#ifndef GLSLMULTIRESMESH_H
#define GLSLMULTIRESMESH_H

#include "GLSLMesh.h"
#include "GLSLProgram.h"
#include "InterfaceOpenNI.h"
#include "Structures.h"

#include <vector>

namespace PixMeshFusion {
#define REAL_WIDTH 640
#define REAL_HEIGHT 480
#define MAX_SCALES 6
#define MAX_SCALES_GRADIENT 9
#define NUM_REDUCE_PROGRAM 6

#define WIDTHS  640,320,160,80,40,20,8,4,1
#define HEIGHTS 480,240,120,60,30,15,6,3,1


#define PBOS 2
class GLSLMultiResMesh;
/*********************************************************************************************************/
class ReductionDriver {
private:
    bool success;

    GLSLMultiResMesh* meshes;

    int num_reduction_level[MAX_SCALES];
    int level_before_reduction[MAX_SCALES][MAX_SCALES_GRADIENT];
    int level_after_reduction[MAX_SCALES][MAX_SCALES_GRADIENT];
    int programs_each_level[MAX_SCALES][MAX_SCALES_GRADIENT];

    int threadx[MAX_SCALES][MAX_SCALES_GRADIENT];
    int thready[MAX_SCALES][MAX_SCALES_GRADIENT];
    int active_scale;

public:
    PixMeshFusion::GLSLProgram programs_reduce[NUM_REDUCE_PROGRAM];

    ~ReductionDriver();
    ReductionDriver() {success=false;}
    ReductionDriver(int w,int h, int scales,std::string shader_path,GLSLMultiResMesh* meshes_);
    void reduce();
    void create(int w,int h, int scales,std::string shader_path,GLSLMultiResMesh* meshes_);
    inline bool isOK(){return success;}
    inline void set_active_scale(int scale){active_scale=scale;}


};
/*********************************************************************************************************/

/*********************************************************************************************************/

class GLSLMultiResMesh {
public:
    friend class ReductionDriver;

    GLSLMultiResMesh(int w, int h, int scales);
    GLSLMultiResMesh();
    ~GLSLMultiResMesh();
    void create(int w, int h, int scales);
    inline void setClassicalGradient(bool classic) {classical=classic;}
    inline void setActiveScale(int s) {active_scale = s;}
    inline void setActiveScaleEnergy(int s) {active_scale_energy = s;}
    inline int getScaleWidth() {return meshes[active_scale].getWidth();}
    inline int getScaleHeight() {return meshes[active_scale].getHeight();}
    inline int getScaleEnergyWidth() {return widths_scale[active_scale_energy];}
    inline int getScaleEnergyHeight() {return heights_scale[active_scale_energy];}
    inline float getFocal() {return meshes[active_scale].getFocal();}
    void setCameraIndex(int cam);
    void loadCamera(float fx,float fy, float skew, float ux, float uy,int width, int height);
    void loadTextureDepth(unsigned short* depth);
    unsigned short* getTextureDepth();
    unsigned short* getTextureDepthTimet();
    unsigned short* getTextureDepthTimetp1();
    float*          getTextureW456Energy() ;
    float*          getTextureW123Energy();
    float*          getTextureNormal() ;


    void loadTextureColor(const openni::RGB888Pixel*  color) ;
    void loadTextureColorDepth(CamData camdata);
    inline void bindMesh() {meshes[active_scale].bindMesh();}
    inline void bindEnergy() {meshes[active_scale].bindEnergy();}
    inline void bindMeshForComputeShader(int binding) {meshes[active_scale].bindMeshForComputeShader(binding);}
    inline void bindEnergy123ForComputeShader(int binding) {meshes[active_scale].bindEnergy123ForComputeShader(binding);}
    inline void bindEnergy456ForComputeShader(int binding) {meshes[active_scale].bindEnergy456ForComputeShader(binding);}
    void bindTexture() ;
    void bindTextureColor(int location, bool read) ;
    void bindTextureEnergyW123Timetp1(int location, bool read) ;
    void bindTextureNormalDdepth(int location, bool read) ;
    void bindTextureAlpha(int location, bool read) ;
    void bindTextureNormalzVisible(int location, bool read) ;
    void bindTextureDepthTimetp1(int location, bool read) ;
    void bindTextureDeltaDepthTimetp1(int location, bool read);

    void bindTextureDepthTimet(int location, bool read) ;
    void bindMeshAndTexture() ;
    inline void drawMesh(bool display) {meshes[active_scale].drawMesh(display);}

    inline void setTextureEnergyW123Timetp1Current() {tidCurrentTexture = tidW123Energy[active_scale_energy];}
    inline void setTextureEnergyW456DdCurrent() {tidCurrentTexture = tidW456Dd_RGBA32f[active_scale_energy];}
    void binCurrentTextureRGBA32F(int location, bool read);
    void attachReprojectionProgram(GLSLProgram* reproj) { glsl_reproj = reproj;}
    inline void attachComputeShaderProgram(GLSLProgram* reproj) { glsl_compute = reproj;}
    inline void attachComputeShaderClassicalProgram(GLSLProgram* reproj) { glsl_compute_classical = reproj;}
    inline void attachComputeShaderClassicalDepthProgram(GLSLProgram* reproj) { glsl_compute_classical_depth = reproj;}

    inline float* getVboData(int& size) {return meshes[active_scale].getVboData(size);};

    void generateMipMap();

    void attachReprojectionProgramColorToIntAndBind(int order);
    void attachReprojectionProgramDepthToIntAndBind(int order);
    void attachReprojectionProgramEnergyToIntAndBind(int order);
    inline void setUniformInvertIntrinsics() {meshes[active_scale].setUniformInvertIntrinsics(glsl_reproj->getHandle());}
    inline void setUniformIntrinsics() {meshes[active_scale].setUniformIntrinsics(glsl_reproj->getHandle());}
    inline void setUniformRotation(float angle, float vx,float vy, float vz) {meshes[active_scale].setUniformRotation(glsl_reproj->getHandle(),angle,vx,vy,vz);}
    inline void setUniformRotation(mat4 RT) {meshes[active_scale].setUniformRotation(glsl_reproj->getHandle(), RT);}
    inline void setUniformTranslation(float vx,float vy, float vz) {meshes[active_scale].setUniformTranslation(glsl_reproj->getHandle(),vx,vy,vz);}
    inline void setUniformInvertIntrinsicsCompute() {
        if(!classical)
            meshes[active_scale].setUniformInvertIntrinsics(glsl_compute->getHandle());
        else
            meshes[active_scale].setUniformInvertIntrinsics(glsl_compute_classical->getHandle());

    }
    inline void setUniformInvertIntrinsicsComputeDepth() {
        if(!classical)
            meshes[active_scale].setUniformInvertIntrinsics(glsl_compute_classical_depth->getHandle());
        else
            meshes[active_scale].setUniformInvertIntrinsics(glsl_compute_classical_depth->getHandle());

    }
    inline void setUniformIntrinsicsCompute() {meshes[active_scale].setUniformIntrinsics(glsl_compute->getHandle());}
    inline void setUniformIntrinsicsFullCompute() {
        if(!classical)
            meshes[active_scale].setUniformIntrinsicsFull(glsl_compute->getHandle());
        else
            meshes[active_scale].setUniformIntrinsicsFull(glsl_compute_classical->getHandle());

    }
    inline void setUniformIntrinsicsFullComputeDepth() {
        if(!classical)
            meshes[active_scale].setUniformIntrinsicsFull(glsl_compute_classical_depth->getHandle());
        else
            meshes[active_scale].setUniformIntrinsicsFull(glsl_compute_classical_depth->getHandle());

    }
    inline void setUniformRotationCompute(mat4 RT) {
        if(!classical)
            meshes[active_scale].setUniformRotation(glsl_compute->getHandle(),RT);
        else
            meshes[active_scale].setUniformRotation(glsl_compute_classical->getHandle(),RT);
    }
    inline void setUniformRotationComputeDepth(mat4 RT) {
        if(!classical)
            meshes[active_scale].setUniformRotation(glsl_compute_classical_depth->getHandle(),RT);
        else
            meshes[active_scale].setUniformRotation(glsl_compute_classical_depth->getHandle(),RT);
    }
    inline void setUniformIntComputeDepth(int iter) {
        if(!classical)
            meshes[active_scale].setUniformInt(glsl_compute_classical_depth->getHandle(),iter);
        else
            meshes[active_scale].setUniformInt(glsl_compute_classical_depth->getHandle(),iter);
    }

    inline void bindFrameBuffer(){
        meshes[active_scale].bindFrameBuffer();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, tiddepthTimet);
    }
    inline void bindFrameBufferEnergy(){meshes[active_scale].bindFrameBufferEnergy();}
    inline void unbindFrameBuffer(){glBindFramebuffer(GL_FRAMEBUFFER, 0);}
    void getW456(float* w456) ;
    void getW123(float* w123) ;
    void AttachDepthTimetToRenderBuffer();
    void AttachEnergyToRenderBuffer();

    inline void updateTime() {time_update++;}
    inline bool started() {return (time_update > 1);}
    void retrieveTextureId() ;
private:
    GLSLProgram* glsl_reproj;
    GLSLProgram* glsl_compute;
    GLSLProgram* glsl_compute_classical;
    GLSLProgram* glsl_compute_classical_depth;

    bool allocated;
    int  num_scales;
    int  active_scale;
    int  active_scale_energy;
    std::vector<GLSLMesh> meshes;


    GLuint          tiddepthOtherTime;
    GLuint          tidcolorOtherTime;

    GLuint          tiddepthTimet;
    GLuint          tidcolorTimet;
    GLuint          tiddepthTimetp1;
    GLuint          tidcolorTimetp1;

    int             time_update;

    GLuint          tidW123Energy[MAX_SCALES_GRADIENT];
    GLuint          tidW456Dd_RGBA32f[MAX_SCALES_GRADIENT];

    GLuint          tidDeltaDdIrgb_RGBA32f;
    GLuint          tidalphaDalphaRGBalphaDV_RGBA32f;

    GLuint          tiddepth;
    GLuint          tidcolor;

    GLuint          tidNormalxyDdepth2d;
    GLuint          tidNormalzVisible;

    GLuint          tidCurrentTexture;

    GLuint          pbo_w123[PBOS];
    GLuint          pbo_w456[PBOS];
    void*           ptr_w123[PBOS];
    void*           ptr_w456[PBOS];
    int             read_idxw123;
    int             write_idxw123;
    int             read_idxw456;
    int             write_idxw456;
    GLuint          pbos_color[PBOS];
    GLuint          pbos_depth[PBOS];
    int             read_idx;
    int             write_idx;
    void*           ptr_color[PBOS];
    void*           ptr_depth[PBOS];
    bool            pbo_working;

    int             w,h;


    int widths_scale[MAX_SCALES_GRADIENT] ={WIDTHS};
    int heights_scale[MAX_SCALES_GRADIENT]={HEIGHTS};

    bool classical;

};

}
/*********************************************************************************************************/
#endif // GLSLMULTIRESMESH_H
