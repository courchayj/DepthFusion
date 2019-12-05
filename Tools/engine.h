/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/

#include "GLSLMultiResMesh.h"
#include "Structures.h"

#ifndef ENGINE_H
#define ENGINE_H

#include "SO3.h"
#include "Variables.h"

#include <iostream>
#include <iomanip>
#include <sstream>
namespace PixMeshFusion {


/*********************************************************************************************************/
class Engine {
public:
    Engine(int w, int h,int scales, string shader_path, bool test=false);
    void loadCameraData(CamData camdata);
    void minimizeEnergy();
    void minimizeGradient(int iter,int step);
    void minimizeGauss();
    inline bool isOK(){return created;}
    inline void setClassical(bool classic) {classical=classic; meshes.setClassicalGradient(classic);}
    inline void setGaussNewton(bool gauss) {gauss_newton=gauss; }
    void SyntheticTransformtTotp1();
    void checkData();
private:

    void initStep(float w123[4],float w456[4],int step);
    void improveStep(float w123[4],float w456[4]);
    void rescaleOmega(float w123[4],float w456[4]);
    bool clampOmega(float w123[4],float w456[4]) ;
    bool clampEnergy(float w123[4],float w456[4]) ;
    void pre_computes();
    void compute_gradients();
    void compute_gradients_whole(int iter);

    GLSLMultiResMesh meshes;
    bool created;
    PixMeshFusion::GLSLProgram compute_visibility;
    PixMeshFusion::GLSLProgram compute_gradient;
    PixMeshFusion::GLSLProgram compute_gradient_classical;
    PixMeshFusion::GLSLProgram compute_gradient_classical_depth;
    PixMeshFusion::GLSLProgram pre_compute;
    PixMeshFusion::GLSLProgram reduce_compute;

    PixMeshFusion::GLSLProgram vertices_compute;
    PixMeshFusion::GLSLProgram compute_depth_visibility;

    CamData                    cam_data;
    ReductionDriver            reduction_driver;
    int w_,h_;
    bool test_;
    int threadx;
    int thready;
    int active_scale;

    float weight123[4];
    float weight456[4];
    float weight_energy;
    float alpha123[4];
    float alpha456[4];

    float alpha123initial[4];
    float alpha456initial[4];

    float old_w123[4];
    float old_w456[4];
    float old_energy;

    bool z_only;
    bool y_only;
    bool x_only;

    bool tx_only;
    bool ty_only;
    bool tz_only;

    int count;

    SO3OpenGL so3_lie;
    bool classical;
    bool gauss_newton;

};
/*********************************************************************************************************/
}
#endif // CAMERA_H
