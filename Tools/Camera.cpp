/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/

#include "Camera.h"

using namespace PixMeshFusion;
/*********************************************************************************************************/
camera::camera(float fx,float fy, float skew, float ux, float uy,int width, int height) {
    create(fx,fy, skew, ux, uy,width, height);
}
/*********************************************************************************************************/
void camera::create(float fx,float fy, float skew, float ux, float uy,int width, int height) {
    w=width;
    h=height;

    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            intrinsics(i,j) = 0.0f;
    intrinsics(0,0) = fx;//ONE MINUS FOR Opencv -> OpenGL
    //https://blog.noctua-software.com/opencv-opengl-projection-matrix.html
    intrinsics(1,1) = -fy;
    intrinsics(0,1) = skew;
    intrinsics(0,2) = (ux+ OFFSET);//ONE MINUS FOR Opencv -> OpenGL
    //https://blog.noctua-software.com/opencv-opengl-projection-matrix.html
    intrinsics(1,2) = (h-uy + OFFSET);//decale 0.5 pixel from opencv to opengl
    intrinsics(2,2) = 1.0f;


}
/*********************************************************************************************************/
void camera::computeOpenGLCamera() {
    mat3 intrinsic;

    intrinsic[0][0] = intrinsics(0,0);
    intrinsic[0][1] = intrinsics(1,0);
    intrinsic[0][2] = intrinsics(2,0);

    intrinsic[1][0] = intrinsics(0,1);
    intrinsic[1][1] = intrinsics(1,1);
    intrinsic[1][2] = intrinsics(2,1);

    intrinsic[2][0] = intrinsics(0,2);
    intrinsic[2][1] = intrinsics(1,2);
    intrinsic[2][2] = intrinsics(2,2);

    //FOR ALL INVERT ADD FUNCTION WITH SKEW FOR INVERT
    intrinsics_glm_cpu =  intrinsic; //map [0,w] not [-1,1]

    //for mapping 0 to w
    intrinsics_inv_glm[0][0]=1/intrinsics(0,0);
    intrinsics_inv_glm[1][0]=0;
    intrinsics_inv_glm[2][0]=-intrinsics(0,2)/intrinsics(0,0);

    intrinsics_inv_glm[0][1]=0;
    intrinsics_inv_glm[1][1]=1/intrinsics(1,1);;
    intrinsics_inv_glm[2][1]=-intrinsics(1,2)/intrinsics(1,1);

    intrinsics_inv_glm[0][2]=0;
    intrinsics_inv_glm[1][2]=0;
    intrinsics_inv_glm[2][2]=intrinsics(2,2);
    //intrinsic to opengl camera
    float n = NEAR;//ADD NEAR FAR
    float f  = FAR;//ADD NEAR FAR
    float cx = intrinsics(0,2) ;
    float cy = intrinsics(1,2) ;
    float alpha = intrinsics(0,0);
    float beta  = intrinsics(1,1);
    //http://kgeorge.github.io/2014/03/08/calculating-opengl-perspective-matrix-from-opencv-intrinsic-matrix
    //https://blog.noctua-software.com/opencv-opengl-projection-matrix.html
    //https://strawlab.org/2011/11/05/augmented-reality-with-OpenGL
    //SKEW TO BE ADDED: https://strawlab.org/2011/11/05/augmented-reality-with-OpenGL

    //map -1 to 1
    intrinsics_gl[0][0] = 2.0 * alpha /float( w);
    intrinsics_gl[1][0] = 0;
    intrinsics_gl[2][0] = -1.0 + (2.0*cx)/float(w)  ;
    intrinsics_gl[3][0] = 0;

    intrinsics_gl[0][1] = 0;
    intrinsics_gl[1][1] = 2.0*beta/float(h);
    intrinsics_gl[2][1] = (-1.0 + 2.0*(cy)/float(h)) ;
    intrinsics_gl[3][1] = 0;

    intrinsics_gl[0][2] = 0;
    intrinsics_gl[1][2]  = 0;
    intrinsics_gl[2][2]  = -intrinsics(2,2)*(f+n)/(n-f);
    intrinsics_gl[3][2]  = 2.0*f*n/(n-f);

    intrinsics_gl[0][3]  = 0;
    intrinsics_gl[1][3] = 0;
    intrinsics_gl[2][3] = intrinsics(2,2);
    intrinsics_gl[3][3] = 0;


    //map 0 to w
    intrinsics_gl_full[0][0] = alpha;
    intrinsics_gl_full[1][0] = 0;
    intrinsics_gl_full[2][0] = cx ;

    intrinsics_gl_full[0][1] = 0;
    intrinsics_gl_full[1][1] = beta;
    intrinsics_gl_full[2][1] = cy ;

    intrinsics_gl_full[0][2] = 0;
    intrinsics_gl_full[1][2]  = 0;
    intrinsics_gl_full[2][2]  = intrinsics(2,2);


}
/*********************************************************************************************************/
