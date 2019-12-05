/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/


#include "GLSLProgram.h"

using namespace PixMeshFusion;

    /*********************************************************************************************************/
    std::string GLSLProgram::readFile(const char *filePath, bool& fileExists) {
        std::string content;
        std::ifstream fileStream(filePath, std::ios::in);
        if(!fileStream.is_open()) {
            fileExists = false;
            logString = logString + "Could not read file " + std::string(filePath) + ". File does not exist.\n";
            return "";
        }
        std::string line = "";
        while(!fileStream.eof()) {
            std::getline(fileStream, line);
            content.append(line + "\n");
        }
        fileStream.close();
        fileExists=true;
        return content;
    }
    /*********************************************************************************************************/
    GLSLProgram::GLSLProgram() {
        handle = glCreateProgram();
        if( 0 == handle ) created =false;
        else created =true;
        linked=false;
        logString="";
    }
    /*********************************************************************************************************/
    GLSLProgram::~GLSLProgram() {
        //delete shaders
        for(size_t i=0;i<shaders.size();i++)
            detachShader(shaders[i]);
        for(size_t i=0;i<shaders.size();i++)
            glDeleteShader(shaders[i]);

        glDeleteProgram(handle);
    }
    /*********************************************************************************************************/
    bool   GLSLProgram::compileAndAttachShaderFromFile( const char * fileName,GLSLShader::GLSLShaderType type ){
        GLuint vertShader =0;
        switch (type)
        {
           case GLSLShader::GLSLShaderType::VERTEX:
                vertShader = glCreateShader( GL_VERTEX_SHADER );
                break;
            case GLSLShader::GLSLShaderType::GEOMETRY:
                vertShader = glCreateShader( GL_GEOMETRY_SHADER );
                break;
            case GLSLShader::GLSLShaderType::FRAGMENT:
                vertShader = glCreateShader( GL_FRAGMENT_SHADER );
                break;
            case GLSLShader::COMPUTE:
                vertShader = glCreateShader(GL_COMPUTE_SHADER);
                break;
            default:
                vertShader = 0;
        }
        if( 0 == vertShader )   return false;
        // Read shader
        bool fileExists;
        std::string vertShaderStr = readFile(fileName,fileExists);
        if(!fileExists)return false;
        const char *vertShaderSrc = vertShaderStr.c_str();
        glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
        glCompileShader(vertShader);
        GLint result;
        glGetShaderiv( vertShader, GL_COMPILE_STATUS, &result );
        if( GL_FALSE == result )
        {
            logString = logString + "Shader (VERTEX, FRAGMENT or GEOMETRY or COMPUTE)compilation failed!\n";
            GLint logLen;
            glGetShaderiv( vertShader, GL_INFO_LOG_LENGTH, &logLen );
            if( logLen > 0 )
            {
                char * log = (char *)malloc(logLen);
                GLsizei written;
                glGetShaderInfoLog(vertShader, logLen, &written, log);
                logString = logString + "Shader log: " +std::string(log) + "\n";
                free(log);
            }
            return false;
         }
         glAttachShader( handle, vertShader );
         shaders.push_back(vertShader);
         return true;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completeVGF(string shader_path) {
        bool compile = false;
        if(isCreated() ) {
            string str0 = "/update_quad.geom";
            string str1 = "/update_mesh.vert";
            string str2 = "/update_render.frag";
            compile = compileAndAttachShaderFromFile((shader_path + str1).c_str(),PixMeshFusion::GLSLShader::VERTEX);
            if(compile)
                compile = compileAndAttachShaderFromFile((shader_path + str0).c_str(),PixMeshFusion::GLSLShader::GEOMETRY);
            if(compile)
                compile = compileAndAttachShaderFromFile((shader_path +str2).c_str(),PixMeshFusion::GLSLShader::FRAGMENT);
            if (compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completeVGFDepth(string shader_path) {
        bool compile = false;
        if(isCreated() ) {
            string str0 = "/transmit_z.geom";
            string str1 = "/transmit_z.vert";
            string str2 = "/transmit_z.frag";
            compile = compileAndAttachShaderFromFile((shader_path + str1).c_str(),PixMeshFusion::GLSLShader::VERTEX);
            if(compile)
                compile = compileAndAttachShaderFromFile((shader_path + str0).c_str(),PixMeshFusion::GLSLShader::GEOMETRY);
            if(compile)
                compile = compileAndAttachShaderFromFile((shader_path +str2).c_str(),PixMeshFusion::GLSLShader::FRAGMENT);
            if (compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completeReduce_16_16(string shader_path) {
        bool compile = false;
        if(isCreated()) {
            string str1 = "/reduce_16_16.comp";
            compile = compileAndAttachShaderFromFile((shader_path +str1).c_str(),PixMeshFusion::GLSLShader::COMPUTE);
            if(compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completeReduce_10_10(string shader_path) {
        bool compile = false;
        if(isCreated()) {
            string str1 = "/reduce_10_10.comp";
            compile = compileAndAttachShaderFromFile((shader_path +str1).c_str(),PixMeshFusion::GLSLShader::COMPUTE);
            if(compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completeReduce_8_8(string shader_path) {
        bool compile = false;
        if(isCreated()) {
            string str1 = "/reduce_8_8.comp";
            compile = compileAndAttachShaderFromFile((shader_path +str1).c_str(),PixMeshFusion::GLSLShader::COMPUTE);
            if(compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completeReduce_5_5(string shader_path) {
        bool compile = false;
        if(isCreated()) {
            string str1 = "/reduce_5_5.comp";
            compile = compileAndAttachShaderFromFile((shader_path +str1).c_str(),PixMeshFusion::GLSLShader::COMPUTE);
            if(compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completeReduce_4_3(string shader_path) {
        bool compile = false;
        if(isCreated()) {
            string str1 = "/reduce_4_3.comp";
            compile = compileAndAttachShaderFromFile((shader_path +str1).c_str(),PixMeshFusion::GLSLShader::COMPUTE);
            if(compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completeReduce_2_2(string shader_path) {
        bool compile = false;
        if(isCreated()) {
            string str1 = "/reduce_2_2.comp";
            compile = compileAndAttachShaderFromFile((shader_path +str1).c_str(),PixMeshFusion::GLSLShader::COMPUTE);
            if(compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completeCompute(string shader_path) {
        bool compile = false;
        if(isCreated()) {
            string str1 = "/compute_gradient.comp";
            compile = compileAndAttachShaderFromFile((shader_path +str1).c_str(),PixMeshFusion::GLSLShader::COMPUTE);
            if(compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completeComputeClassical(string shader_path) {
        bool compile = false;
        if(isCreated()) {
            string str1 = "/compute_gradient_classical.comp";
            compile = compileAndAttachShaderFromFile((shader_path +str1).c_str(),PixMeshFusion::GLSLShader::COMPUTE);
            if(compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completeComputeClassicalDepth(string shader_path) {
        bool compile = false;
        if(isCreated()) {
            string str1 = "/compute_gradient_classical_depth.comp";
            compile = compileAndAttachShaderFromFile((shader_path +str1).c_str(),PixMeshFusion::GLSLShader::COMPUTE);
            if(compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completeVerticesCompute(string shader_path) {
        bool compile = false;
        if(isCreated()) {
            string str1 = "/compute_vertices.comp";
            compile = compileAndAttachShaderFromFile((shader_path +str1).c_str(),PixMeshFusion::GLSLShader::COMPUTE);
            if(compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool GLSLProgram::completePreCompute(string shader_path) {
        bool compile = false;
        if(isCreated()) {
            string str1 = "/pre_compute.comp";
            compile = compileAndAttachShaderFromFile((shader_path +str1).c_str(),PixMeshFusion::GLSLShader::COMPUTE);
            if(compile) {
                compile = link();
            }
        }
        return compile;
    }
    /*********************************************************************************************************/
    bool   GLSLProgram::link(){
        if(created) glLinkProgram( handle );
        else return false;
        GLint status;
        glGetProgramiv( handle, GL_LINK_STATUS, &status );
        if( GL_FALSE == status ) {
            fprintf( stderr, "Failed to link shader program!\n" );
            GLint logLen;
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logLen);
            if( logLen > 0 )
            {
                char * log = (char *)malloc(logLen);
                GLsizei written;
                glGetProgramInfoLog(handle, logLen,&written, log);
                fprintf(stderr, "Program log: \n%s", log);
                free(log);
            }
            linked=false;
            return false;
        }
        linked = true;
        glValidateProgram(handle);
        return true;
    }
    /*********************************************************************************************************/
    void   GLSLProgram::use(){
        if(linked)  glUseProgram( handle );
    }
    /*********************************************************************************************************/
    GLuint GLSLProgram::getHandle() {return handle;}
    bool   GLSLProgram::isLinked()  {return linked;}
    bool   GLSLProgram::isCreated() {return created;}
    string GLSLProgram::log()       {return logString;}
    /*********************************************************************************************************/
    void GLSLProgram::bindAttribLocation( GLuint location,const char * name){
        glBindAttribLocation(handle, location,name);
    }
    /*********************************************************************************************************/
    void GLSLProgram::bindFragDataLocation( GLuint location, const char * name ){
        glBindFragDataLocation(handle, location,name);
    }
    /*********************************************************************************************************/
    void GLSLProgram::printActiveUniforms(){}
    void GLSLProgram::printActiveAttribs(){}
    /*********************************************************************************************************/
    void drawMeshes() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES,0,3*(100+1)*(100+1));
        //glDrawArrays(GL_POINTS, 0, 100);
    }
    /*********************************************************************************************************/
    int GLSLShader::GLinit( int argc, char ** argv,const char* name, int width,int height) {
        glutInit(&argc, argv);

        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
        glutInitWindowSize(width, height);
        glutInitWindowPosition(100, 100);
        int win = glutCreateWindow(name);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
        glutSwapBuffers();
        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
          /* Problem: glewInit failed, something is seriously wrong. */
          fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        }
        fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
        return win;
    }
    /*********************************************************************************************************/
    void GLSLProgram::printWorkGroupsCapabilities() {
      int workgroup_count[3];
      int workgroup_size[3];
      int workgroup_invocations;

      glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workgroup_count[0]);
      glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workgroup_count[1]);
      glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workgroup_count[2]);

      printf ("Taille maximale des workgroups:\n\tx:%u\n\ty:%u\n\tz:%u\n",
      workgroup_size[0], workgroup_size[1], workgroup_size[2]);

      glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workgroup_size[0]);
      glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workgroup_size[1]);
      glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workgroup_size[2]);

      printf ("Nombre maximal d'invocation locale:\n\tx:%u\n\ty:%u\n\tz:%u\n",
      workgroup_size[0], workgroup_size[1], workgroup_size[2]);

      glGetIntegerv (GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workgroup_invocations);
      printf ("Nombre maximum d'invocation de workgroups:\n\t%u\n", workgroup_invocations);
    }
    /*********************************************************************************************************/
