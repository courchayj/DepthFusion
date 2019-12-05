#ifndef MAIN_H
#define MAIN_H
/*
* This file was written for controling PixelFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/
#include <iostream>
#include <fstream>

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>

#include <Tools/GLSLProgram.h>
#include <Tools/GLSLMesh.h>
#include <Tools/GLSLMultiResMesh.h>
#include <Tools/Camera.h>
#include <Tools/SO3.h>
#include <Tools/InterfaceOpenNI.h>
#include <Tools/Structures.h>
#include <Tools/engine.h>

#include <time.h>



#endif // MAIN_H





/*
 * #version 430 compatibility

layout (local_size_x = 16, local_size_y = 16, local_size_z=1) in;

layout(location = 2) uniform mat3 intrinsics_inv;

layout (r16, binding = 0) uniform image2D img_depthTimet;//input

layout (rgba32f, binding = 1) uniform image2D img_normalxyDdepth2d;//output


void main() {

  ivec2 coords = ivec2(gl_GlobalInvocationID);

  ivec2 coordsxm = ivec2(coords.x-1,coords.y);
  ivec2 coordsxp = ivec2(coords.x+1,coords.y);
  ivec2 coordsym = ivec2(coords.x,coords.y-1);
  ivec2 coordsyp = ivec2(coords.x,coords.y+1);

  float dataxm = coords.x-1;//imageLoad(img_depthTimet, coordsxm).r;
  float dataxp = coords.x+1;//imageLoad(img_depthTimet, coordsxp).r;
  float dataym = coords.y-1;//imageLoad(img_depthTimet, coordsym).r;
  float datayp = coords.y+1;//imageLoad(img_depthTimet, coordsyp).r;

  vec3 delta1 = intrinsics_inv * (vec3((coordsxm.x+0.5)*dataxm ,(coordsxm.y+0.5)*dataxm,dataxm )  - vec3((coordsxp.x+0.5)*dataxp ,(coordsxp.y+0.5)*dataxp,dataxp ));
  vec3 delta2 = intrinsics_inv * (vec3((coordsym.x+0.5)*dataym ,(coordsym.y+0.5)*dataym,dataym )  - vec3((coordsyp.x+0.5)*datayp ,(coordsyp.y+0.5)*datayp,datayp ));

  vec3 normal = normalize(cross(delta1,delta2));
  normal = normal * sign(normal.z) * sign(dataxm)*sign(dataxp)*sign(dataym)*sign(datayp);

  imageStore(img_normalxyDdepth2d, coords, vec4(normal.x,normal.y,sign(dataxp)*sign(dataxm) *(dataxp-dataxm)/2.0,sign(datayp)*sign(dataym)*(datayp-dataym)/2.0));

}
 */


/*
#version 430 compatibility

layout (local_size_x = 16, local_size_y = 16, local_size_z=1) in;

layout(location = 2) uniform mat3 intrinsics_inv;

layout (r16, binding = 0) uniform image2D img_depthTimet;//input

layout (rgba32f, binding = 1) uniform image2D img_normalxyDdepth2d;//output

shared float localData[gl_WorkGroupSize.x+2][gl_WorkGroupSize.y+2];

void main() {
    //use shared memory to accelerate
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);


    localData   [gl_LocalInvocationID.x+1][gl_LocalInvocationID.y+1] =  imageLoad(img_depthTimet, coords).r;

    if((gl_LocalInvocationID.x==0) && (gl_LocalInvocationID.y==0)) {
        localData   [gl_LocalInvocationID.x][gl_LocalInvocationID.y] =  imageLoad(img_depthTimet, coords-ivec2(1,1)).r;
        localData   [gl_LocalInvocationID.x][gl_LocalInvocationID.y+1] =  imageLoad(img_depthTimet, coords-ivec2(1,0)).r;
        localData   [gl_LocalInvocationID.x+1][gl_LocalInvocationID.y] =  imageLoad(img_depthTimet, coords-ivec2(0,1)).r;
    }
    else if((gl_LocalInvocationID.x==gl_WorkGroupSize.x-1) && (gl_LocalInvocationID.y==gl_WorkGroupSize.y-1)) {
        localData   [gl_LocalInvocationID.x+2][gl_LocalInvocationID.y+2] =  imageLoad(img_depthTimet, coords+ivec2(1,1)).r;
        localData   [gl_LocalInvocationID.x+2][gl_LocalInvocationID.y+1] =  imageLoad(img_depthTimet, coords+ivec2(1,0)).r;
        localData   [gl_LocalInvocationID.x+1][gl_LocalInvocationID.y+2] =  imageLoad(img_depthTimet, coords+ivec2(0,1)).r;
    }
    else if((gl_LocalInvocationID.x==0) && (gl_LocalInvocationID.y==gl_WorkGroupSize.y-1)) {
        localData   [gl_LocalInvocationID.x][gl_LocalInvocationID.y+1] =  imageLoad(img_depthTimet, coords+ivec2(-1,0)).r;
        localData   [gl_LocalInvocationID.x][gl_LocalInvocationID.y+2] =  imageLoad(img_depthTimet, coords+ivec2(-1,1)).r;
        localData   [gl_LocalInvocationID.x+1][gl_LocalInvocationID.y+2] =  imageLoad(img_depthTimet, coords+ivec2(0,1)).r;
    }
    else if((gl_LocalInvocationID.x==gl_WorkGroupSize.x-1) && (gl_LocalInvocationID.y==0)) {
        localData   [gl_LocalInvocationID.x+1][gl_LocalInvocationID.y] =  imageLoad(img_depthTimet, coords+ivec2(0,-1)).r;
        localData   [gl_LocalInvocationID.x+2][gl_LocalInvocationID.y] =  imageLoad(img_depthTimet, coords+ivec2(1,-1)).r;
        localData   [gl_LocalInvocationID.x+2][gl_LocalInvocationID.y+1] =  imageLoad(img_depthTimet, coords+ivec2(1,0)).r;
    }
    else if((gl_LocalInvocationID.x==0) ) {
        localData   [gl_LocalInvocationID.x][gl_LocalInvocationID.y+1] =  imageLoad(img_depthTimet, coords+ivec2(-1,0)).r;
    }
    else if((gl_LocalInvocationID.y==0) ) {
        localData   [gl_LocalInvocationID.x+1][gl_LocalInvocationID.y] =  imageLoad(img_depthTimet, coords+ivec2(0,-1)).r;
    }
    else if((gl_LocalInvocationID.y==gl_WorkGroupSize.y-1) ) {
        localData   [gl_LocalInvocationID.x+1][gl_LocalInvocationID.y+2] =  imageLoad(img_depthTimet, coords+ivec2(0,1)).r;
    }
    else if((gl_LocalInvocationID.x==gl_WorkGroupSize.x-1) ) {
        localData   [gl_LocalInvocationID.x+2][gl_LocalInvocationID.y+1] =  imageLoad(img_depthTimet, coords+ivec2(1,0)).r;
    }

    barrier();

    uvec2 p = gl_LocalInvocationID.xy + uvec2(1,1);

    float dataxm = localData[p.x-1][p.y];
    float dataxp = localData[p.x+1][p.y];
    float dataym = localData[p.x][p.y-1];
    float datayp = localData[p.x][p.y+1];

    //ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 coordsxm = ivec2(coords.x-1,coords.y);
    ivec2 coordsxp = ivec2(coords.x+1,coords.y);
    ivec2 coordsym = ivec2(coords.x,coords.y-1);
    ivec2 coordsyp = ivec2(coords.x,coords.y+1);

    vec3 delta1 = intrinsics_inv * (vec3((coordsxm.x+0.5)*dataxm ,(coordsxm.y+0.5)*dataxm,dataxm )  - vec3((coordsxp.x+0.5)*dataxp ,(coordsxp.y+0.5)*dataxp,dataxp ));
    vec3 delta2 = intrinsics_inv * (vec3((coordsym.x+0.5)*dataym ,(coordsym.y+0.5)*dataym,dataym )  - vec3((coordsyp.x+0.5)*datayp ,(coordsyp.y+0.5)*datayp,datayp ));

    vec3 normal = normalize(cross(delta1,delta2));
    normal = normal * sign(normal.z) * sign(dataxm)*sign(dataxp)*sign(dataym)*sign(datayp);

    imageStore(img_normalxyDdepth2d, ivec2(gl_GlobalInvocationID.xy), vec4(normal.x,normal.y,sign(dataxp)*sign(dataxm) *(dataxp-dataxm)/2.0,sign(datayp)*sign(dataym)*(datayp-dataym)/2.0));

}
 */

