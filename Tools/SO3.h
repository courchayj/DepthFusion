/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/

#ifndef SO3_H
#define SO3_H

#include "matrix.h"
#include "math.h"


#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/transform2.hpp>
#include <iostream>
#include <fstream>


namespace PixMeshFusion {
//lie algebra equtions and derivations
/*********************************************************************************************************/
class SO3 {
public:
   float w[6];
   SO3();
   matrix<4,4> getRT() {return RT;}
   matrix<3,6> getJacobian(){return Jdydw;}
   void increaseSO3(float dw[6]) {for(int i=0;i<6;i++) w[i]+=dw[i];}
   void updateSO3(float dw[6]) {for(int i=0;i<6;i++) w[i]=dw[i];}
   void computeRT();
   void computeJacobian(float y[3] );
   inline void reinit(){w[0]=0.0;w[1]=0.0;w[2]=0.0;w[3]=0.0;w[4]=0.0;w[5]=0.0;}
private:   
    matrix<4,4> RT;
    matrix<3,6> Jdydw; //y = Rx
};
/*********************************************************************************************************/


/*********************************************************************************************************/
class SO3OpenGL {
public:
   float w123Energy[4];
   float w456Dd[4];
   SO3OpenGL() {w123Energy[0]=0.0;w123Energy[1]=0.0;w123Energy[2]=0.0;w456Dd[0]=0.0;w456Dd[1]=0.0;w456Dd[2]=0.0; RT = glm::mat4(1.0);new_RT = glm::mat4(1.0);old_RT = glm::mat4(1.0);}
   inline glm::mat4 getRT() {return RT;}
   void printRT();
   void computeRT();
   inline void reinit(){w123Energy[0]=0.0;w123Energy[1]=0.0;w123Energy[2]=0.0;w456Dd[0]=0.0;w456Dd[1]=0.0;w456Dd[2]=0.0;RT = glm::mat4(1.0);new_RT = glm::mat4(1.0);old_RT = glm::mat4(1.0);}
private:
   glm::mat4 RT;
   glm::mat4 old_RT;
   glm::mat4 new_RT;
};
/*********************************************************************************************************/
}
#endif // SO3_H
