/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/
#include "GLSLMultiResMesh.h"

using namespace PixMeshFusion ;
/*********************************************************************************************************/
GLSLMultiResMesh::GLSLMultiResMesh() {
    allocated =false;
}
/*********************************************************************************************************/
void GLSLMultiResMesh::create(int width, int height, int scales) {
    w=width;
    h=height;
    scales = max(MAX_SCALES,1);//at least one scale
    allocated=false;
    /*
    meshes.resize(scales);
    for(int s=0;s<scales;s++) {
        int scale_divide = int(pow(2.0,s) +0.1);
        meshes[s].create(w/scale_divide,h/scale_divide,scale_divide);
    }
    */
    active_scale = 0;
    num_scales = scales;

    //at instant time t (not t+1) already improved

    for(int i=0;i<MAX_SCALES_GRADIENT;i++) {

        glGenTextures(1,&tidW456Dd_RGBA32f[i]);
        glBindTexture(GL_TEXTURE_2D,tidW456Dd_RGBA32f[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, widths_scale[i],
        heights_scale[i], 0,GL_RGBA, GL_FLOAT,0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_2D);
    }



    glGenTextures(1,&tidDeltaDdIrgb_RGBA32f);
    glBindTexture(GL_TEXTURE_2D, tidDeltaDdIrgb_RGBA32f);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w,
                 h, 0,GL_RGBA, GL_FLOAT,0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, num_scales-1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1,&tidalphaDalphaRGBalphaDV_RGBA32f);
    glBindTexture(GL_TEXTURE_2D, tidalphaDalphaRGBalphaDV_RGBA32f);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w,
                 h, 0,GL_RGBA, GL_FLOAT,0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, num_scales-1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_2D);


    glGenTextures(1,&tidNormalzVisible);
    glBindTexture(GL_TEXTURE_2D, tidNormalzVisible);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, w,
                 h, 0,GL_RG, GL_FLOAT,0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, num_scales-1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_2D);



    glGenTextures(1,&tidNormalxyDdepth2d);
    glBindTexture(GL_TEXTURE_2D, tidNormalxyDdepth2d);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w,
                 h, 0,GL_RGBA, GL_FLOAT,0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, num_scales-1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_2D);
    //two instant time

    glGenTextures(1,&tiddepth);
    glBindTexture(GL_TEXTURE_2D, tiddepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, w,
    h, 0,GL_RED, GL_UNSIGNED_SHORT,0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, num_scales-1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1,&tidcolor);
    glBindTexture(GL_TEXTURE_2D, tidcolor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w,
    h, 0,GL_RGB, GL_UNSIGNED_BYTE,0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, num_scales-1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);

    for(int i=0;i<MAX_SCALES_GRADIENT;i++) {

        glGenTextures(1,&tidW123Energy[i]);
        glBindTexture(GL_TEXTURE_2D,tidW123Energy[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, widths_scale[i],
        heights_scale[i], 0,GL_RGBA, GL_FLOAT,0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    //other instant time

    glGenTextures(1,&tiddepthOtherTime);
    glBindTexture(GL_TEXTURE_2D, tiddepthOtherTime);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, w,
    h, 0,GL_RED, GL_UNSIGNED_SHORT,0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, num_scales-1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1,&tidcolorOtherTime);
    glBindTexture(GL_TEXTURE_2D, tidcolorOtherTime);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w,
    h, 0,GL_RGB, GL_UNSIGNED_BYTE,0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, num_scales-1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glGenBuffers(PBOS,pbos_color);
    glGenBuffers(PBOS,pbos_depth);
    read_idx=write_idx=0;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos_depth[0]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, w * h * sizeof(unsigned short), NULL,  GL_STREAM_DRAW);
    ptr_depth[0] = (void*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos_depth[1]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, w * h * sizeof(unsigned short),  NULL, GL_STREAM_DRAW);
    ptr_depth[1] = (void*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos_color[0]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, w * h * 3* sizeof(unsigned char),  NULL,GL_STREAM_DRAW);
    ptr_color[0] = (void*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos_color[1]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, w * h * 3* sizeof(unsigned char),  NULL, GL_STREAM_DRAW);
    ptr_color[1] = (void*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);


    glGenBuffers(PBOS,pbo_w123);
    glGenBuffers(PBOS,pbo_w456);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_w123[0]);
    glBufferData(GL_PIXEL_PACK_BUFFER, 4*sizeof(float), NULL, GL_STREAM_COPY);
    ptr_w123[0] = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_w456[0]);
    glBufferData(GL_PIXEL_PACK_BUFFER, 4*sizeof(float), NULL, GL_STREAM_COPY);
    ptr_w456[0] = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_w123[1]);
    glBufferData(GL_PIXEL_PACK_BUFFER, 4*sizeof(float), NULL, GL_STREAM_COPY);
    ptr_w123[1] = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_w456[1]);
    glBufferData(GL_PIXEL_PACK_BUFFER, 4*sizeof(float), NULL, GL_STREAM_COPY);
    ptr_w456[1] = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    read_idxw123=write_idxw123=0;
    read_idxw456=write_idxw456=0;

    if(ptr_color[0] && ptr_color[1] && ptr_depth[0] && ptr_depth[1] )
        pbo_working=true;
    else
        pbo_working=false;

    pbo_working=false;//remove ping pong loading

    //create meshes
    meshes.resize(scales);
    for(int s=0;s<scales;s++) {
        int scale_divide = int(pow(2.0,s) +0.1);
        meshes[s].create(w/scale_divide,h/scale_divide,scale_divide);
        setActiveScale(s);//IMPORTANT
        AttachDepthTimetToRenderBuffer();
        unbindFrameBuffer();
        AttachEnergyToRenderBuffer();
        unbindFrameBuffer();
    }

    allocated =true;

    //init time
    time_update =0;
    active_scale=0;
    active_scale_energy=0;
}
/*********************************************************************************************************/
GLSLMultiResMesh::GLSLMultiResMesh(int width, int height, int scales) {
        create(width,height,scales);
}
/*********************************************************************************************************/
void  GLSLMultiResMesh::retrieveTextureId()  {
    if (time_update%2==0) {
        tidcolorTimet = tidcolorOtherTime;
        tiddepthTimet = tiddepthOtherTime;

        tidcolorTimetp1 = tidcolor;
        tiddepthTimetp1 = tiddepth;
    }
    else {
        tidcolorTimet = tidcolor;
        tiddepthTimet = tiddepth;

        tidcolorTimetp1 = tidcolorOtherTime;
        tiddepthTimetp1 = tiddepthOtherTime;
    }
}
/*********************************************************************************************************/
void GLSLMultiResMesh::setCameraIndex(int cam_index) {
    for(int s=0;s<num_scales;s++)   meshes[s].setCameraIndex(cam_index);
}
/*********************************************************************************************************/
void GLSLMultiResMesh::loadCamera(float fx,float fy, float skew, float ux, float uy,int width, int height)
{
    for(int s=0;s<num_scales;s++) {
        meshes[s].loadCamera(fx,fy, skew, ux,  uy, width,  height);
    }
}
/*********************************************************************************************************/
void GLSLMultiResMesh::bindTexture() {
    //glActiveTexture(GL_TEXTURE0 + order_depth);
    glBindTexture(GL_TEXTURE_2D, tiddepthTimetp1);

    //glActiveTexture(GL_TEXTURE0 + order_color);
    glBindTexture(GL_TEXTURE_2D, tidcolorTimetp1);
}
/*********************************************************************************************************/
void GLSLMultiResMesh::binCurrentTextureRGBA32F(int location, bool read) {
    glBindTexture(GL_TEXTURE_2D, tidCurrentTexture);
    if(read)
        glBindImageTexture (location, tidCurrentTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    else
        glBindImageTexture (location, tidCurrentTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}
/*********************************************************************************************************/
void GLSLMultiResMesh::bindTextureColor(int location, bool read) {
    glBindTexture(GL_TEXTURE_2D, tidcolorTimetp1);
    if(read)
        glBindImageTexture (location, tidcolorTimetp1, active_scale, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    else
        glBindImageTexture (location, tidcolorTimetp1, active_scale, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}
/*********************************************************************************************************/
void GLSLMultiResMesh::bindTextureEnergyW123Timetp1(int location, bool read) {
    glBindTexture(GL_TEXTURE_2D, tidW123Energy[active_scale_energy]);
    if(read)
        glBindImageTexture (location, tidW123Energy[active_scale_energy],0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    else
        glBindImageTexture (location, tidW123Energy[active_scale_energy], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}
/*********************************************************************************************************/
void GLSLMultiResMesh::bindTextureDeltaDepthTimetp1(int location, bool read) {
    glBindTexture(GL_TEXTURE_2D, tidDeltaDdIrgb_RGBA32f);
    if(read)
        glBindImageTexture (location, tidDeltaDdIrgb_RGBA32f, active_scale, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    else
        glBindImageTexture (location, tidDeltaDdIrgb_RGBA32f, active_scale, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}
/*********************************************************************************************************/
void GLSLMultiResMesh::bindTextureAlpha(int location, bool read) {
    glBindTexture(GL_TEXTURE_2D, tidalphaDalphaRGBalphaDV_RGBA32f);
    if(read)
        glBindImageTexture (location, tidalphaDalphaRGBalphaDV_RGBA32f, active_scale, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    else
        glBindImageTexture (location, tidalphaDalphaRGBalphaDV_RGBA32f, active_scale, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}
/*********************************************************************************************************/
void GLSLMultiResMesh::bindTextureDepthTimetp1(int location, bool read) {
    glBindTexture(GL_TEXTURE_2D, tiddepthTimetp1);
    if(read)
        glBindImageTexture (location, tiddepthTimetp1, active_scale, GL_FALSE, 0, GL_READ_WRITE, GL_R16);
    else
        glBindImageTexture (location, tiddepthTimetp1, active_scale, GL_FALSE, 0, GL_READ_WRITE, GL_R16);
}
/*********************************************************************************************************/
void GLSLMultiResMesh::bindTextureDepthTimet(int location, bool read) {
    glBindTexture(GL_TEXTURE_2D, tiddepthTimet);
    if(read)
        glBindImageTexture (location, tiddepthTimet, active_scale, GL_FALSE, 0, GL_READ_ONLY, GL_R16);
    else
        glBindImageTexture (location, tiddepthTimet, active_scale, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16);
}
/*********************************************************************************************************/
void GLSLMultiResMesh:: bindTextureNormalDdepth(int location, bool read) {
    glBindTexture(GL_TEXTURE_2D, tidNormalxyDdepth2d);
    if(read)
        glBindImageTexture (location, tidNormalxyDdepth2d, active_scale, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    else
        glBindImageTexture (location, tidNormalxyDdepth2d, active_scale, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}
/*********************************************************************************************************/
void GLSLMultiResMesh:: bindTextureNormalzVisible(int location, bool read) {
    glBindTexture(GL_TEXTURE_2D, tidNormalzVisible);
    if(read)
        glBindImageTexture (location, tidNormalzVisible, active_scale, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
    else
        glBindImageTexture (location, tidNormalzVisible, active_scale, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
}
/*********************************************************************************************************/
void GLSLMultiResMesh::bindMeshAndTexture() {
    meshes[active_scale].bindMesh();
    //glActiveTexture(GL_TEXTURE0 +  order_depth);
    glBindTexture(GL_TEXTURE_2D, tiddepthTimetp1);

    //glActiveTexture(GL_TEXTURE0 +  order_color);
    glBindTexture(GL_TEXTURE_2D, tidcolorTimetp1);
}
/*********************************************************************************************************/
void GLSLMultiResMesh::loadTextureDepth(unsigned short* depth) {
    glBindTexture(GL_TEXTURE_2D, tiddepthTimetp1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, w,
    h, GL_RED, GL_UNSIGNED_SHORT,depth);

}
/*********************************************************************************************************/
void GLSLMultiResMesh::loadTextureColorDepth(CamData camdata) {
    //PBO Multiple texture loading asynchronous better
    if(pbo_working) {
        read_idx = write_idx;
        write_idx = 1-write_idx;


        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos_depth[read_idx]);
        glBindTexture(GL_TEXTURE_2D, tiddepthTimetp1);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, w,
                        h, GL_RED, GL_UNSIGNED_SHORT,0);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos_depth[write_idx]);
        //glBufferData(GL_PIXEL_UNPACK_BUFFER, w * h * sizeof(unsigned short), NULL, GL_STREAM_DRAW);
        //ptr = (void*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        memcpy(ptr_depth[write_idx], (void*)camdata.depthData, w*h*sizeof(unsigned short));
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos_color[read_idx]);
        glBindTexture(GL_TEXTURE_2D, tidcolorTimetp1);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, w,
                        h, GL_RGB, GL_UNSIGNED_BYTE,0);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos_color[write_idx]);
        //glBufferData(GL_PIXEL_UNPACK_BUFFER, 3*w * h * sizeof(unsigned char), NULL, GL_STREAM_DRAW);
        //ptr = (void*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        memcpy(ptr_color[write_idx], (void*)camdata.colorData, 3*w*h*sizeof(unsigned char));
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, tiddepthTimetp1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, w,
        h, GL_RED, GL_UNSIGNED_SHORT,camdata.depthData);
        glBindTexture(GL_TEXTURE_2D, tidcolorTimetp1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, w,
        h, GL_RGB, GL_UNSIGNED_BYTE,camdata.colorData);
    }

}
/*********************************************************************************************************/
unsigned short* GLSLMultiResMesh::getTextureDepth()
{
    unsigned short* frame_data = new unsigned short[w*h];

    meshes[active_scale].bindFrameBuffer();
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels( 0,0,  w, h, GL_RED, GL_UNSIGNED_SHORT, frame_data);
    meshes[active_scale].unbindFrameBuffer();

    return frame_data;
}
/*********************************************************************************************************/
unsigned short* GLSLMultiResMesh::getTextureDepthTimet() {
    unsigned short* frame_data = new unsigned short[w*h];
    glBindTexture(GL_TEXTURE_2D, tiddepthTimet);
    glGetTexImage(GL_TEXTURE_2D,active_scale,GL_RED,GL_UNSIGNED_SHORT,frame_data);
    return frame_data;
}
/*********************************************************************************************************/
unsigned short* GLSLMultiResMesh::getTextureDepthTimetp1() {
    unsigned short* frame_data = new unsigned short[w*h];
    glBindTexture(GL_TEXTURE_2D, tiddepthTimetp1);
    glGetTexImage(GL_TEXTURE_2D,active_scale,GL_RED,GL_UNSIGNED_SHORT,frame_data);
    return frame_data;
}
/*********************************************************************************************************/
float* GLSLMultiResMesh::getTextureW456Energy() {
    float* frame_data = new float[4*getScaleEnergyWidth()*getScaleEnergyHeight()];
    glBindTexture(GL_TEXTURE_2D, tidW456Dd_RGBA32f[active_scale_energy]);
    glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_FLOAT,frame_data);
    return frame_data;
}
/*********************************************************************************************************/
float* GLSLMultiResMesh::getTextureW123Energy() {
    float* frame_data = new float[4*getScaleEnergyWidth()*getScaleEnergyHeight()];
    glBindTexture(GL_TEXTURE_2D, tidW123Energy[active_scale_energy]);
    glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_FLOAT,frame_data);
    return frame_data;
}
/*********************************************************************************************************/
float* GLSLMultiResMesh::getTextureNormal() {
    float* frame_data = new float[4*getScaleEnergyWidth()*getScaleEnergyHeight()];
    glBindTexture(GL_TEXTURE_2D, tidNormalxyDdepth2d);
    glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_FLOAT,frame_data);
    return frame_data;
}
/*********************************************************************************************************/
void GLSLMultiResMesh::loadTextureColor(const openni::RGB888Pixel*  color) {
    glBindTexture(GL_TEXTURE_2D, tidcolorTimetp1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, w,
    h, GL_RGB, GL_UNSIGNED_BYTE,color);
}
/*********************************************************************************************************/
void GLSLMultiResMesh::generateMipMap()
{
    //time t+1
    glBindTexture(GL_TEXTURE_2D, tidcolorTimetp1);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tiddepthTimetp1);
    glGenerateMipmap(GL_TEXTURE_2D);
}
/*********************************************************************************************************/
GLSLMultiResMesh::~GLSLMultiResMesh()
{
    if(allocated) {
        glDeleteTextures(1,&tidDeltaDdIrgb_RGBA32f);
        glDeleteTextures(1,&tidalphaDalphaRGBalphaDV_RGBA32f);

        glDeleteTextures(1,&tidNormalxyDdepth2d);
        glDeleteTextures(1,&tidNormalzVisible);

        glDeleteTextures(MAX_SCALES_GRADIENT,tidW123Energy);
        glDeleteTextures(MAX_SCALES_GRADIENT,tidW456Dd_RGBA32f);

        glDeleteTextures(1,&tiddepth);
        glDeleteTextures(1,&tidcolor);
        glDeleteTextures(1,&tiddepthOtherTime);
        glDeleteTextures(1,&tidcolorOtherTime);

        glDeleteBuffers(PBOS,pbo_w123);
        glDeleteBuffers(PBOS,pbo_w456);
        glDeleteBuffers(PBOS,pbos_color);
        glDeleteBuffers(PBOS,pbos_depth);
    }
    allocated =false;
}
/*********************************************************************************************************/
void GLSLMultiResMesh::AttachDepthTimetToRenderBuffer() {
    bindFrameBuffer();
    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tiddepthTimet, active_scale);

    GLuint st = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(st){
        case GL_FRAMEBUFFER_COMPLETE:
            //std::cout << "GL_FRAMEBUFFER_COMPLETE" <<std::endl;
            break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" <<std::endl;
            break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT    :
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" <<std::endl;
            break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" <<std::endl;
            break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" <<std::endl;
            break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" <<std::endl;
            break;
           case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" <<std::endl;
            break;
            case GL_FRAMEBUFFER_UNDEFINED :
            std::cout << "GL_FRAMEBUFFER_UNDEFINED" <<std::endl;
            break;
    }

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
}
/*********************************************************************************************************/
void GLSLMultiResMesh::getW123(float* w123) {
    read_idxw123 = write_idxw123;
    write_idxw123 = 1-write_idxw123;


    glBindTexture(GL_TEXTURE_2D, tidW123Energy[MAX_SCALES_GRADIENT-1]);
    //glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_w123[read_idxw123]);

    glGetTexImage(GL_TEXTURE_2D,
                 0,
                  GL_RGBA,
                  GL_FLOAT,
                  w123);

    //do som stuff if wanted
    //glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_w123[read_idxw123]);
    //void* mappedBuffer = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    ////now mapped buffer contains the pixel data
    //memcpy(w123,ptr_w123[write_idxw123],  4*sizeof(float));
    //glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

    //glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

}
/*********************************************************************************************************/
void GLSLMultiResMesh::getW456(float* w456) {
    read_idxw456 = write_idxw456;
    write_idxw456 = 1-write_idxw456;


    glBindTexture(GL_TEXTURE_2D, tidW456Dd_RGBA32f[MAX_SCALES_GRADIENT-1]);
    //glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_w456[read_idxw456]);

    glGetTexImage(GL_TEXTURE_2D,
                 0,
                  GL_RGBA,
                  GL_FLOAT,
                  w456);

    //do som stuff if wanted
    //glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_w456[read_idxw456]);
    //void* mappedBuffer = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    ////now mapped buffer contains the pixel data
    //memcpy(w456,ptr_w456[write_idxw456],  4*sizeof(float));
    //glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

    //glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);


}
/*********************************************************************************************************/
void GLSLMultiResMesh::AttachEnergyToRenderBuffer() {
    bindFrameBufferEnergy();
    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tidW123Energy[active_scale],0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, tidW456Dd_RGBA32f[active_scale],0);

    GLuint st = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(st){
        case GL_FRAMEBUFFER_COMPLETE:
            //std::cout << "GL_FRAMEBUFFER_COMPLETE" <<std::endl;
            break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" <<std::endl;
            break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT    :
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" <<std::endl;
            break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" <<std::endl;
            break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" <<std::endl;
            break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" <<std::endl;
            break;
           case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" <<std::endl;
            break;
            case GL_FRAMEBUFFER_UNDEFINED :
            std::cout << "GL_FRAMEBUFFER_UNDEFINED" <<std::endl;
            break;
    }

    // Set the list of draw buffers.
    GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, DrawBuffers); // "1" is the size of DrawBuffers
}
/*********************************************************************************************************/
void GLSLMultiResMesh::attachReprojectionProgramColorToIntAndBind(int order){
    int order_color =order;
    glActiveTexture(GL_TEXTURE0 + order_color);
    glBindTexture(GL_TEXTURE_2D, tidcolorTimetp1);
    meshes[active_scale].attachReprojectionProgramColorToInt(glsl_reproj->getHandle(),order);
}
/*********************************************************************************************************/
void GLSLMultiResMesh::attachReprojectionProgramDepthToIntAndBind(int order){
    int order_depth =order;
    glActiveTexture(GL_TEXTURE0 + order_depth);
    glBindTexture(GL_TEXTURE_2D, tiddepthTimetp1);
    meshes[active_scale].attachReprojectionProgramDepthToInt(glsl_reproj->getHandle(),order);

}
/*********************************************************************************************************/
void GLSLMultiResMesh::attachReprojectionProgramEnergyToIntAndBind(int order){
    int order_energy =order;
    glActiveTexture(GL_TEXTURE0 + order_energy);
    glBindTexture(GL_TEXTURE_2D, tidW123Energy[active_scale]);
    meshes[active_scale].attachReprojectionProgramEnergyToInt(glsl_reproj->getHandle(),order);

}
/*********************************************************************************************************/



/*********************************************************************************************************/
ReductionDriver::~ReductionDriver() {
    //delete programs_reduce; Not Pointer anymore
}
/*********************************************************************************************************/
ReductionDriver::ReductionDriver(int w,int h, int scales,std::string shader_path,GLSLMultiResMesh* meshes_) {
    create(w,h,scales,shader_path,meshes_);
}
/*********************************************************************************************************/
void ReductionDriver::create(int w,int h, int scales,std::string shader_path,GLSLMultiResMesh* meshes_) {
    meshes = meshes_;//store multi res mesh pointer for further computation
    success=true;
    if(scales >MAX_SCALES) success=false;
    if(w!= REAL_WIDTH) success=false;
    if(h!= REAL_HEIGHT) success=false;

    if(!success)    std::cout << "reduction driver failed!" << std::endl;
    bool compile;
    compile = programs_reduce[0].completeReduce_16_16(shader_path);
    if(!compile) {
            success=false;
            programs_reduce[0].printLog();
    }
    compile = programs_reduce[1].completeReduce_10_10(shader_path);
    if(!compile) {
            success=false;
            programs_reduce[1].printLog();
    }
    compile = programs_reduce[2].completeReduce_8_8(shader_path);
    if(!compile) {
            success=false;
            programs_reduce[2].printLog();
    }
    compile = programs_reduce[3].completeReduce_5_5(shader_path);
    if(!compile) {
            success=false;
            programs_reduce[3].printLog();
    }
    compile = programs_reduce[4].completeReduce_2_2(shader_path);
    if(!compile) {
            success=false;
            programs_reduce[4].printLog();
    }
    compile = programs_reduce[5].completeReduce_4_3(shader_path);
    if(!compile) {
            success=false;
            programs_reduce[5].printLog();
    }

    num_reduction_level[0]=3;
    num_reduction_level[1]=3;
    num_reduction_level[2]=3;
    num_reduction_level[3]=3;
    num_reduction_level[4]=2;
    num_reduction_level[5]=2;

    level_before_reduction[0][0]=0;
    level_after_reduction[0][0]=4;
    level_before_reduction[0][1]=4;
    level_after_reduction[0][1]=7;
    level_before_reduction[0][2]=7;
    level_after_reduction[0][2]=8;

    level_before_reduction[1][0]=1;
    level_after_reduction[1][0]=5;
    level_before_reduction[1][1]=5;
    level_after_reduction[1][1]=7;
    level_before_reduction[1][2]=7;
    level_after_reduction[1][2]=8;

    level_before_reduction[2][0]=2;
    level_after_reduction[2][0]=5;
    level_before_reduction[2][1]=5;
    level_after_reduction[2][1]=7;
    level_before_reduction[2][2]=7;
    level_after_reduction[2][2]=8;

    level_before_reduction[3][0]=3;
    level_after_reduction[3][0]=6;
    level_before_reduction[3][1]=6;
    level_after_reduction[3][1]=7;
    level_before_reduction[3][2]=7;
    level_after_reduction[3][2]=8;

    level_before_reduction[4][0]=4;
    level_after_reduction[4][0]=7;
    level_before_reduction[4][1]=7;
    level_after_reduction[4][1]=8;

    level_before_reduction[5][0]=5;
    level_after_reduction[5][0]=7;
    level_before_reduction[5][1]=7;
    level_after_reduction[5][1]=8;

    programs_each_level[0][0]=0;
    programs_each_level[0][1]=1;
    programs_each_level[0][2]=5;

    programs_each_level[1][0]=0;
    programs_each_level[1][1]=3;
    programs_each_level[1][2]=5;

    programs_each_level[2][0]=2;
    programs_each_level[2][1]=3;
    programs_each_level[2][2]=5;

    programs_each_level[3][0]=1;
    programs_each_level[3][1]=4;
    programs_each_level[3][2]=5;

    programs_each_level[4][0]=1;
    programs_each_level[4][1]=5;

    programs_each_level[5][0]=3;
    programs_each_level[5][1]=5;

    threadx[0][0] = 16;
    thready[0][0] = 16;
    threadx[0][1] = 10;
    thready[0][1] = 10;
    threadx[0][2] = 4;
    thready[0][2] = 3;

    threadx[1][0] = 16;
    thready[1][0] = 16;
    threadx[1][1] = 5;
    thready[1][1] = 5;
    threadx[1][2] = 4;
    thready[1][2] = 3;

    threadx[2][0] = 8;
    thready[2][0] = 8;
    threadx[2][1] = 5;
    thready[2][1] = 5;
    threadx[2][2] = 4;
    thready[2][2] = 3;

    threadx[3][0] = 10;
    thready[3][0] = 10;
    threadx[3][1] = 2;
    thready[3][1] = 2;
    threadx[3][2] = 4;
    thready[3][2] = 3;

    threadx[4][0] = 10;
    thready[4][0] = 10;
    threadx[4][1] = 4;
    thready[4][1] = 3;

    threadx[5][0] = 5;
    thready[5][0] = 5;
    threadx[5][1] = 4;
    thready[5][1] = 3;
}
/*********************************************************************************************************/
void ReductionDriver::reduce() {

    for(int lev=0;lev<num_reduction_level[active_scale];lev++) {
        programs_reduce[programs_each_level[active_scale][lev]].use();
        meshes->setActiveScaleEnergy(level_before_reduction[active_scale][lev]);
        meshes->setTextureEnergyW123Timetp1Current();
        meshes->binCurrentTextureRGBA32F(1,true);
        meshes->setTextureEnergyW456DdCurrent();
        meshes->binCurrentTextureRGBA32F(3,true);
        meshes->setActiveScaleEnergy(level_after_reduction[active_scale][lev]);
        meshes->setTextureEnergyW123Timetp1Current();
        meshes->binCurrentTextureRGBA32F(2,false);
        meshes->setTextureEnergyW456DdCurrent();
        meshes->binCurrentTextureRGBA32F(4,false);
        meshes->setActiveScaleEnergy(level_before_reduction[active_scale][lev]);
        glDispatchCompute(meshes->getScaleEnergyWidth()/threadx[active_scale][lev],meshes->getScaleEnergyHeight()/thready[active_scale][lev],1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
}
/*********************************************************************************************************/
