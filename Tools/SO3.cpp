/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/

#include "SO3.h"

using namespace PixMeshFusion;

/*********************************************************************************************************/
SO3::SO3() {
 for(int i=0;i<6;i++)
     w[i]=0.0f;
 for(int i=0;i<4;i++)
     for(int j=0;j<4;j++)
         RT(i,j)=0.0f;
 for(int i=0;i<4;i++)
         RT(i,i)=1.0f;
 RT(3,3) = 1.0f;
}
/*********************************************************************************************************/
void SO3::computeRT(){
    //compute w^
    matrix<3,3> w_mat;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            w_mat(i,j)=0.0f;
    w_mat(2,1)=w[3];
    w_mat(1,2)=-w[3];

    w_mat(2,0)=-w[4];
    w_mat(0,2)=w[4];

    w_mat(1,0)=w[5];
    w_mat(0,1)=-w[5];

    matrix<3,1> u;
    u(0,0) = w[0];
    u(1,0) = w[1];
    u(2,0) = w[2];
    //compute w^2
    matrix<3,3> w_mat_square = w_mat*w_mat;

    float theta = sqrt(w[3]*w[3] + w[4]*w[4] + w[5]*w[5]);
    float A = sin(theta)/theta;
    float B = (1-cos(theta)) / (theta*theta);
    float C = (1 - A)/ (theta*theta);

    matrix<3,3> I;
    for(int i=0;i<3;i++) I(i,i)=1.0f;

    matrix<3,3> R = I + w_mat * A + w_mat_square * B;
    matrix<3,3> V = I + w_mat * B + w_mat_square * C;

    matrix<3,1> Vu = V*u;

    //store result
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            RT(i,j) = R(i,j);
    for(int i=0;i<3;i++)
        RT(i,3) = Vu(i,0);

}
/*********************************************************************************************************/
void SO3::computeJacobian(float y[3] ){//y = RT x dy/dw theta en theta=0 => Recentrer autour de theta =0
    //identity on the left
    for(int i=0;i<3;i++)
        for(int j=0;j<6;j++)
            Jdydw(i,j)=0.0f;
    for(int i=0;i<3;i++)
        Jdydw(i,i)=1.0f;
    //-y cross product on the right
    Jdydw(2,1+3)=y[0];
    Jdydw(1,2+3)=-y[0];

    Jdydw(2,0+3)=-y[1];
    Jdydw(0,2+3)=y[1];

    Jdydw(0,1+3)=-y[2];
    Jdydw(1,0+3)=y[2];
}
/*********************************************************************************************************/
void SO3OpenGL::computeRT(){
    //compute w^
    glm::mat3 w_mat = glm::mat3(0.0f);

    w_mat[1][2]=-w456Dd[0];
    w_mat[2][1]=w456Dd[0];

    w_mat[0][2]=w456Dd[1];
    w_mat[2][0]=-w456Dd[1];

    w_mat[0][1]=-w456Dd[2];
    w_mat[1][0]=w456Dd[2];

    glm::vec3 u;
    u[0] = w123Energy[0];
    u[1] = w123Energy[1];
    u[2] = w123Energy[2];
    //compute w^2
    glm::mat3 w_mat_square = w_mat*w_mat;

    float theta = sqrt(w456Dd[0]*w456Dd[0] + w456Dd[1]*w456Dd[1] + w456Dd[2]*w456Dd[2]);

    //std::cout << "theta" << theta << std::endl;
    //std::cout << "epsilon" << FLT_MIN << std::endl;

    float A = sin(theta)/theta;
    float B = (1-cos(theta)) / (theta*theta);
    float C = (1 - A)/ (theta*theta);

    if(theta <FLT_MIN)    {A=1.0;B=1.0/2.0;C=1.0f/6.0;}
    //if(theta <FLT_MIN) {A=0.0;B=0.0;C=0.0f;}

    glm::mat3 I = glm::mat3(1.0f);

    glm::mat3 R = I + w_mat * A + w_mat_square * B;
    glm::mat3 V = I + w_mat * B + w_mat_square * C;

    glm::vec3 Vu = V*u;

    //store result
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            new_RT[i][j] = R[i][j];
    for(int i=0;i<3;i++)
        new_RT[3][i] = Vu[i];

    RT = new_RT * RT;
    //RT = new_RT * old_RT;
    //old_RT = RT;

}
/*********************************************************************************************************/
void SO3OpenGL::printRT() {
    std::cout << "Rotation Matrix " << std::endl;
    std::cout << RT[0][0] << " " << RT[1][0]<< " " << RT[2][0]<< " " << RT[3][0] << std::endl;
    std::cout << RT[0][1] << " " << RT[1][1]<< " " << RT[2][1]<< " " << RT[3][1] << std::endl;
    std::cout << RT[0][2] << " " << RT[1][2]<< " " << RT[2][2]<< " " << RT[3][2] << std::endl;
    std::cout << RT[0][3] << " " << RT[1][3]<< " " << RT[2][3]<< " " << RT[3][3] << std::endl;
}
