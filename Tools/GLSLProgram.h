/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/

#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H
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

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;
using namespace std;

namespace PixMeshFusion {

namespace GLSLShader {
    enum GLSLShaderType {
        VERTEX, FRAGMENT, GEOMETRY, COMPUTE
        };
    int GLinit( int argc, char ** argv,const char* name,int width,int height);
}
//classe non abstraite
class GLSLProgram {
private:
    std::vector<GLuint> shaders;
    GLuint              handle;
    bool                linked;
    bool                created;
    string              logString;
    std::string readFile(const char *filePath, bool& fileExists);
public:
    ~GLSLProgram();
    GLSLProgram();
    void detachShader(GLuint shader) { glDetachShader(handle,shader);}
    bool compileAndAttachShaderFromFile( const char * fileName,GLSLShader::GLSLShaderType type );
    bool completeVGF(string shader_path);
    bool completeVGFDepth(string shader_path) ;
    bool completeVerticesCompute(string shader_path);
    bool completeCompute(string shader_path);
    bool completeComputeClassical(string shader_path);
    bool completeComputeClassicalDepth(string shader_path) ;

    bool completePreCompute(string shader_path);

    bool completeReduce_16_16(string shader_path) ;
    bool completeReduce_10_10(string shader_path) ;
    bool completeReduce_8_8(string shader_path) ;
    bool completeReduce_5_5(string shader_path) ;
    bool completeReduce_2_2(string shader_path) ;
    bool completeReduce_4_3(string shader_path) ;

    bool link();
    void use();
    string log();
    GLuint getHandle();
    bool isLinked();
    bool isCreated();
    //virtual void computeMeshFromDepth() {}  // = 0 signifie "virtuelle pure"

    void printLog() {std::cout << logString << std::endl;}
    void bindAttribLocation( GLuint location,const char * name);
    void bindFragDataLocation( GLuint location, const char * name );

    void printWorkGroupsCapabilities() ;

    void printActiveUniforms();
    void printActiveAttribs();
};
}
/*********************************************************************************************************/

#endif // GLSLPROGRAM_H
