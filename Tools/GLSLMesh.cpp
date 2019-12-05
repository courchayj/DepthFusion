/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/

#include "GLSLMesh.h"
using namespace PixMeshFusion;

//texture interpolation: 0, 0.5, ????
/*********************************************************************************************************/
void GLSLMesh::free() {
    if(allocated) {
        delete vertices;
        delete indices;
        delete uv;

        glDeleteFramebuffers(1, &fboHandle);
        glDeleteRenderbuffers(1,&depthrenderbuffer);
        glDeleteFramebuffers(1, &fboHandleEnergy);
        glDeleteRenderbuffers(1,&depthrenderbufferEnergy);

        glDeleteVertexArrays(1,&vao);
        glDeleteBuffers(1,&vbo);
        glDeleteBuffers(1,&tbo);
        glDeleteBuffers(1,&ibo);

        glDeleteBuffers(1,&ebo123);
        glDeleteBuffers(1,&ebo456);
    }
    allocated=false;
}
/*********************************************************************************************************/
GLSLMesh::~GLSLMesh() {
    free();
}
/*********************************************************************************************************/
void GLSLMesh::attachOpenGLCamera(mat4 intrinsics_glf) {
    intrinsics_gl = intrinsics_glf;
}
/*********************************************************************************************************/
void GLSLMesh::drawMesh(bool display) {
    glViewport(0,0,w,h);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, 6*(w-1)*(h-1),GL_UNSIGNED_INT,(void*)0);//use ibo

    if(display) {
        glutSwapBuffers();//slow only for DISPLAY!!!!!!!!
    }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}
/*********************************************************************************************************/
void GLSLMesh::create(int width, int height,int scale_div) {
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS) ;
    scale_divide = scale_div;
    if(allocated) free();
    w =width;
    h=height;


    //vec3 -> vec4????
    //pixel turned to a mesh
    //create vertices: quadrangle
    vertices = new float[4*(w)*(h)];//num vertices X,Y,Z,PIXEL AT THE MIDDLE
    for(int i=0;i<w;i++)
        for(int j=0;j<h;j++) {
            vertices[4*(i+j*(w))]   = float(i- (w)/2 +OFFSET)/float((w)/2);
            vertices[4*(i+j*(w))+1] = float(j - (h)/2 +OFFSET)/float((h)/2);
            vertices[4*(i+j*(w))+2] = 0;
            vertices[4*(i+j*(w))+3] = 1;
        }
    //create indices
    indices = new unsigned int[(w-1)*(h-1)*6]; //4 * num facets

    for(int i=0;i<w-1;i++)
        for(int j=0;j<h-1;j++) {
            indices[6*(i + j * (w-1))] = i + j*(w);
            indices[1+6*(i + j * (w-1))] = i +1 + (j)*(w);
            indices[2+6*(i + j * (w-1))] = i+1  + (j+1)*(w);

            indices[3+6*(i + j * (w-1))] = i + j*(w);
            indices[4+6*(i + j * (w-1))] = i+1  + (j+1)*(w);
            indices[5+6*(i + j * (w-1))] = i  + (j+1)*(w);

            //indices[3+4*(i + j * (w-1))] = i  + (j+1)*(w);
        }

    //create texture coordinates
    uv = new float[2*(w)*(h)]; //num vertices
    for(int i=0;i<w;i++)
        for(int j=0;j<h;j++) {
            uv[2*(i+j*(w))]   = (float(i) +OFFSET)/(float(w));//DECALE
            uv[2*(i+j*(w))+1] = (float(j) +OFFSET)/(float(h));//DECALE
        }

    //send data to gpu
    glGenVertexArrays( 1, &vao );
    glBindVertexArray(vao);

    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*(w)*(h), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0,
    (GLubyte *)NULL );
    glEnableVertexAttribArray(0); // Vertex position

    glGenBuffers(1,&tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*(w)*(h), uv, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0,
    (GLubyte *)NULL );
    glEnableVertexAttribArray(1); // Vertex texture

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*(w-1)*(h-1)*6, indices, GL_STATIC_DRAW);

    float* energy = new float[4*(w)*(h)];//num vertices X,Y,Z,PIXEL AT THE MIDDLE
    glGenBuffers(1,&ebo123);
    glBindBuffer(GL_ARRAY_BUFFER, ebo123);
    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(float)*(w)*(h), energy, GL_STATIC_DRAW);
    glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, 0,
    (GLubyte *)NULL );
    glEnableVertexAttribArray(2); // Normal position

    glGenBuffers(1,&ebo456);
    glBindBuffer(GL_ARRAY_BUFFER, ebo456);
    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(float)*(w)*(h), energy, GL_STATIC_DRAW);
    glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, 0,
    (GLubyte *)NULL );
    glEnableVertexAttribArray(3); // Normal position

    delete energy;

    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glGenRenderbuffers(1, &depthrenderbuffer);

    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    glGenFramebuffers(1, &fboHandleEnergy);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandleEnergy);
    glGenRenderbuffers(1, &depthrenderbufferEnergy);

    glGenRenderbuffers(1, &depthrenderbufferEnergy);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbufferEnergy);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbufferEnergy);

    current_texture = 0;//count_texture;//deprecated
    count_texture = count_texture+1;//deprecated
    allocated =true;
}
/*********************************************************************************************************/
float* GLSLMesh::getVboData(int& size) {
    glBindBuffer(GL_ARRAY_BUFFER, ebo123);
    size = 4*(w)*(h);
    float* buf = (float*)malloc(sizeof(float) * 1 * size);
    glGetBufferSubData(GL_ARRAY_BUFFER,0,sizeof(float) * 1 * size,buf);
    return buf;
}
/*********************************************************************************************************/
void GLSLMesh::attachReprojectionProgramColorToInt(GLuint handle, int order)
{
    int uniloc = glGetUniformLocation(handle, "TexColor");
    if( uniloc >= 0 )
    glUniform1i(uniloc,order);
    //order_color =order; in multires
}
/*********************************************************************************************************/
void GLSLMesh::attachReprojectionProgramDepthToInt(GLuint handle, int order)
{
    int uniloc = glGetUniformLocation(handle, "TexDepth");
    if( uniloc >= 0 )
    glUniform1i(uniloc,order);
    //order_depth =order; in multires
}
/*********************************************************************************************************/
void GLSLMesh::attachReprojectionProgramEnergyToInt(GLuint handle, int order)
{
    int uniloc = glGetUniformLocation(handle, "TexEnergy");
    if( uniloc >= 0 )
    glUniform1i(uniloc,order);
    //order_depth =order; in multires
}
/*********************************************************************************************************/
void GLSLMesh::setUniformInvertIntrinsics(GLuint handle)
{
    //std::cout << handle << std::endl;
    GLuint location =glGetUniformLocation(handle,"intrinsics_inv");
    //std::cout << location << std::endl;
    //std::cout << intrinsics_inv_glm[0][0] << std::endl;
    if( location >= 0 )
    {
    glUniformMatrix3fv(location, 1, GL_FALSE,&intrinsics_inv_glm[0][0]);
    }
}
/*********************************************************************************************************/
void GLSLMesh::setUniformRotation(GLuint handle,float angle,float vecx, float vecy, float vecz)
{
    //std::cout << "INTRINSICS" << std::endl;
    GLuint location =glGetUniformLocation(handle,"rotation");
    mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, vec3(vecx,vecy,vecz));

    //std::cout << "Rotation " << location << std::endl;
    //std::cout << rotation[0][0] << std::endl;
    //std::cout << rotation[2][2] << std::endl;
    //std::cout << rotation[3][3] << std::endl;
    if( location >= 0 )
    {
    glUniformMatrix4fv(location, 1, GL_FALSE,&rotation[0][0]);
    }
}
/*********************************************************************************************************/
void GLSLMesh::setUniformRotation(GLuint handle,mat4 RT)
{
    GLuint location =glGetUniformLocation(handle,"rotation");

    if( location >= 0 )
    {
    glUniformMatrix4fv(location, 1, GL_FALSE,&RT[0][0]);
    }
}
/*********************************************************************************************************/
void GLSLMesh::setUniformInt(GLuint handle,int iter)
{
    GLuint location =glGetUniformLocation(handle,"iter");

    if( location >= 0 )
    {
    glUniform1i(location, iter);
    }
}
/*********************************************************************************************************/
void GLSLMesh::setUniformTranslation(GLuint handle,float vecx, float vecy, float vecz)
{
    //std::cout << "INTRINSICS" << std::endl;
    GLuint location =glGetUniformLocation(handle,"rotation");
    float nrm=sqrt(0.2*0.2+0.6*0.6+0.25*0.25);
    mat4 rotation = glm::rotate(glm::mat4(1.0f), -6.15f, vec3(0.2/nrm,0.6/nrm,0.25/nrm));
    //rotation = glm::translate(rotation,vec3(0.002,0.001,0.002));
    rotation = glm::translate(rotation,vec3(0.00002,0.00001,0.00002));
    //std::cout << "Rotation " << location << std::endl;
    //std::cout << rotation[0][0] << std::endl;
    //std::cout << rotation[2][2] << std::endl;
    //std::cout << rotation[3][3] << std::endl;
    if( location >= 0 )
    {
    glUniformMatrix4fv(location, 1, GL_FALSE,&rotation[0][0]);
    }
}
/*********************************************************************************************************/
void GLSLMesh::setUniformIntrinsics(GLuint handle)
{
    //DEPRECATED
    GLuint location =glGetUniformLocation(handle,"intrinsics");
    if( location >= 0 )
    {
    glUniformMatrix4fv(location, 1, GL_FALSE,&intrinsics_gl[0][0]);
    }
}
/*********************************************************************************************************/
void GLSLMesh::setUniformIntrinsicsFull(GLuint handle)
{
    GLuint location =glGetUniformLocation(handle,"intrinsics_full");
    if( location >= 0 )
    {
    glUniformMatrix3fv(location, 1, GL_FALSE,&intrinsics_gl_full[0][0]);
    }
}
/*********************************************************************************************************/
void GLSLMesh::loadCamera(float fx,float fy, float skew, float ux, float uy,int width, int height)
{
    intrinsics.create(fx/float(scale_divide),fy/float(scale_divide),skew/float(scale_divide),
                      ux/float(scale_divide),uy/float(scale_divide),width/scale_divide,height/scale_divide);
    intrinsics.computeOpenGLCamera();
    intrinsics_gl     = intrinsics.getOpenGLCamera();
    intrinsics_gl_full= intrinsics.getOpenGLCameraFull();
    intrinsics_inv_glm= intrinsics.getInvertOpenGLCamera();

}
/*********************************************************************************************************/
GLSLMesh::GLSLMesh(int width, int height,int scale_div) {
    create(width,height,scale_div);
}
/*********************************************************************************************************/
void GLSLMesh::bindMesh(){
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0,
    (GLubyte *)NULL );
    glEnableVertexAttribArray(0); // Vertex position
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0,
    (GLubyte *)NULL );
    glEnableVertexAttribArray(1); // Texture position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}
/*********************************************************************************************************/
void GLSLMesh::bindEnergy(){
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, ebo123);
    glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, 0,
    (GLubyte *)NULL );
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, ebo456);
    glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, 0,
    (GLubyte *)NULL );
    glEnableVertexAttribArray(3);
}
/*********************************************************************************************************/
void GLSLMesh::bindMeshForComputeShader(int binding) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,vbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,binding,vbo);

}
/*********************************************************************************************************/
void GLSLMesh::bindEnergy123ForComputeShader(int binding) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,ebo123);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,binding,ebo123);
}
/*********************************************************************************************************/
void GLSLMesh::bindEnergy456ForComputeShader(int binding) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,ebo456);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,binding,ebo456);
}
/*********************************************************************************************************/

