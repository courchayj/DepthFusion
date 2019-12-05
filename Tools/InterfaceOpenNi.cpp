/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/

#include "InterfaceOpenNI.h"


using namespace PixMeshFusion;
using namespace std;
/*********************************************************************************************************/
openni::DepthPixel* InterfaceOpenNI::getDepthData() {
    depth.readFrame(&frame_depth);
    return (openni::DepthPixel*)frame_depth.getData();
}
/*********************************************************************************************************/
openni::RGB888Pixel* InterfaceOpenNI::getColorData() {
    color.readFrame(&frame_color);
    return (openni::RGB888Pixel*)frame_color.getData();
}
 /*********************************************************************************************************/
 CamData InterfaceOpenNI::getColorDepthData(bool& received) {
     CamData camdata;
     color.readFrame(&frame_color);
     camdata.colorData = (openni::RGB888Pixel*)frame_color.getData();
     depth.readFrame(&frame_depth);
     camdata.depthData = (openni::DepthPixel*)frame_depth.getData();
     received =true;
     return camdata;
 }
 /*********************************************************************************************************/
InterfaceOpenNI::InterfaceOpenNI(FPS fps,  WIDTH width, bool print) {
    connected=false;
    openni::Status rc;
    rc=openni::OpenNI::initialize();
    if(rc!=openni::STATUS_OK){
        printf("Error initialize ! %d\n\n",rc);
        openni::OpenNI::shutdown();
        connected = false;
        return;
    }

    rc = device.open(openni::ANY_DEVICE);
    if(rc!=openni::STATUS_OK){
        printf("Error open device! %d\n\n",rc);
        connected=false;
        return;
    }
    rc = depth.create(device, openni::SENSOR_DEPTH);
    if(rc!=openni::STATUS_OK){
        printf("Error create depth! %d\n\n",rc);
        connected=false;
        return;
    }
    rc = color.create(device, openni::SENSOR_COLOR);
    if(rc!=openni::STATUS_OK){
        printf("Error create color! %d\n\n",rc);
        connected=false;
        return;
    }


    const openni::SensorInfo* sinfo = device.getSensorInfo(openni::SENSOR_DEPTH); // select index=4 640x480, 30 fps, 1mm
        if (sinfo == NULL)
        {
            printf("Couldn't get device info\n%s\n", openni::OpenNI::getExtendedError());
            return ;
        }

    int max_depth_res=0;
    int max_depth_mode=0;
    const openni::Array< openni::VideoMode>& modesDepth = sinfo->getSupportedVideoModes();
    for (int i = 0; i < modesDepth.getSize(); i++) {
        if (print) {
            printf("%i: %ix%i, %i fps, %i format\n", i, modesDepth[i].getResolutionX(), modesDepth[i].getResolutionY(),
                modesDepth[i].getFps(), modesDepth[i].getPixelFormat()); //PIXEL_FORMAT_DEPTH_1_MM = 100, PIXEL_FORMAT_DEPTH_100_UM
        }
        if( modesDepth[i].getResolutionX()* modesDepth[i].getResolutionY() > max_depth_res && (modesDepth[i].getFps()==fps) &&
                (modesDepth[i].getResolutionX()==width || (width==NO_WIDTH))
                && (modesDepth[i].getPixelFormat()==openni::PIXEL_FORMAT_DEPTH_1_MM)) {
            max_depth_res =modesDepth[i].getResolutionX()* modesDepth[i].getResolutionY();
            max_depth_mode =i;
        }
    }


    const openni::SensorInfo* cinfo = device.getSensorInfo(openni::SENSOR_COLOR); // select index=4 640x480, 30 fps, 1mm
        if (sinfo == NULL)
        {
            printf("Couldn't get device info\n%s\n", openni::OpenNI::getExtendedError());
            return ;
        }
    int max_color_res=0;
    int max_color_mode=0;
    const openni::Array< openni::VideoMode>& modesVideo = cinfo->getSupportedVideoModes();
    for (int i = 0; i < modesVideo.getSize(); i++) {
        if (print) {
            printf("%i: %ix%i, %i fps, %i format\n", i, modesVideo[i].getResolutionX(), modesVideo[i].getResolutionY(),
                modesVideo[i].getFps(), modesVideo[i].getPixelFormat()); //PIXEL_FORMAT_DEPTH_1_MM = 100, PIXEL_FORMAT_DEPTH_100_UM
        }
        if( modesVideo[i].getResolutionX()* modesVideo[i].getResolutionY() > max_color_res && (modesVideo[i].getFps()==fps) && (modesVideo[i].getResolutionX()==width || (width==NO_WIDTH))
            ) {
            max_color_res =modesVideo[i].getResolutionX()* modesVideo[i].getResolutionY();
            max_color_mode =i;
        }
    }
    if (print) {
        cout << "max depth mode " << max_depth_mode << endl;
        cout << "max color mode " << max_color_mode << endl;
    }
    rc = color.setVideoMode(modesVideo[max_color_mode]);
    if(rc!=openni::STATUS_OK){
        printf("Error set video mode! %d\n\n",rc);
        connected=false;
    }
    rc = depth.setVideoMode(modesDepth[max_depth_mode]);
    if(rc!=openni::STATUS_OK){
        printf("Error set video mode! %d\n\n",rc);
        connected=false;
    }

    connected=true;
}
/*********************************************************************************************************/
void  InterfaceOpenNI::start() {


    if(openni::STATUS_OK != depth.start()) {
        connected=false;
    }
    if(openni::STATUS_OK != color.start()) {
        connected=false;
    }

    if(!connected) return;
    vm_depth = depth.getVideoMode();
    vm_color = color.getVideoMode();

    w_depth=vm_depth.getResolutionX();
    h_depth=vm_depth.getResolutionY();

    w_color=vm_color.getResolutionX();
    h_color=vm_color.getResolutionY();

}
/*********************************************************************************************************/
void  InterfaceOpenNI::stop() {
    depth.stop();
    color.stop();
}
/*********************************************************************************************************/
void Dataset_Fribourg::display() {
    if(created) {
        for(size_t i=0;i<depth_path.size();i++) {
            std::cout << depth_path[i] << std::endl;
            std::cout << image_path[i] << std::endl;
        }
    }
}
/*********************************************************************************************************/
bool Dataset_Fribourg::create(std::string folder) {
    created=false;
    index=50;

    string path_depth = folder + "depth.txt";
    string path_rgb = folder + "rgb.txt";

    ifstream myfile (path_depth);
    if (myfile.is_open())
    {
        string line;
        std::getline(myfile, line);//comment
        std::getline(myfile, line);//comment
        std::getline(myfile, line);//comment

        string a, b;
        while (myfile >> a >> b)
        {
            // process pair (a,b)
            std::string path = folder + b;
            depth_path.push_back(path);
        }
        myfile.close();
    }
    else return false;

    ifstream myfile2 (path_rgb);
    if (myfile2.is_open())
    {
        string line;
        std::getline(myfile2, line);//comment
        std::getline(myfile2, line);//comment
        std::getline(myfile2, line);//comment

        string a, b;
        while (myfile2 >> a >> b)
        {
            // process pair (a,b)
            std::string path = folder + b;
            image_path.push_back(path);
        }
        myfile2.close();
    }
    else return false;

    created=true;
    return true;
}
/*********************************************************************************************************/
CamData Dataset_Fribourg::getColorDepthData() {
    CamData camdata;

    if(index<depth_path.size()) {
        depth = cv::imread(depth_path[index],-1);
        //depth.convertTo(depth,CV_16U,1.0);


        //depth = depth*5000.0;//meter

        //double min, max;
        //cv::minMaxLoc(depth, &min, &max);
        //std::cout << "min" << min << std::endl;
        //std::cout << "max" << max << std::endl;
        //cv::imshow("test",1*depth);

        img = cv::imread(image_path[index]);
    }

    //color.readFrame(&frame_color);
    camdata.colorData = (openni::RGB888Pixel*)img.data;
    //depth.readFrame(&frame_depth);
    camdata.depthData = (openni::DepthPixel*)depth.data;
    //received =true;
    index+=1;//step of motion
    return camdata;


}
/*********************************************************************************************************/
