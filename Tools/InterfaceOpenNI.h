/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr).
*
*/

#ifndef INTERFACEOPENNI_H
#define INTERFACEOPENNI_H

#include <openni2/OpenNI.h>
#include <vector>

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>

#include <Tools/Structures.h>

#include <iostream>
#include <fstream>
#include <string>

namespace PixMeshFusion {

enum FPS {
    FPS_30 = 30, FPS_60 = 60, FPS_15 = 15
    };
enum WIDTH {
    WIDTH_640 = 640, WIDTH_320 = 320, NO_WIDTH=0
    };
/*********************************************************************************************************/
class InterfaceOpenNI {
public:
       ~InterfaceOpenNI() {  openni::OpenNI::shutdown();  }
       InterfaceOpenNI(FPS fps, WIDTH width, bool print);
       void start();
       void stop();
       int getColorWidth() {return w_color;}
       int getColorHeight() {return h_color;}
       int getDepthWidth() {return w_depth;}
       int getDepthHeight() {return h_depth;}
       bool isOK() {return connected;}
       openni::DepthPixel* getDepthData() ;
       openni::RGB888Pixel* getColorData() ;
       CamData getColorDepthData(bool& received);

private:
       openni::Device device;
       openni::VideoStream depth, color;

       openni::VideoFrameRef frame_color;
       openni::VideoFrameRef frame_depth;

       openni::VideoMode vm_color;
       openni::VideoMode vm_depth;

       int  w_depth,h_depth;
       int  w_color,h_color;
       bool connected;
};
/*********************************************************************************************************/
class Dataset_Fribourg {
public:
    Dataset_Fribourg() {};
    bool create(std::string folder);
    void display();
    bool not_ended() {return (index<depth_path.size());}
    CamData getColorDepthData();
private:
    std::vector<std::string> depth_path;
    std::vector<std::string> image_path;
    size_t index;
    bool created;
    cv::Mat img;
    cv::Mat depth;
};
/*********************************************************************************************************/
}

#endif // INTERFACEOPENNI_H
