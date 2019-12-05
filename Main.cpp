/*
* This file was written for controling PixelFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/

#include "Main.h"

int PixMeshFusion::GLSLMesh::count_texture =0;

void Do_Some_Checkings(openni::DepthPixel* dData, PixMeshFusion::GLSLMultiResMesh& meshes,int h, int w);


int main(int argc, char * argv[])
{
    if (argc <2) {
        std::cout <<"Give path to Shaders folder..." << std::endl;
        std::cout <<"Give Path to images folder if required (not device)..." << std::endl;
        return -1;
    }

    PixMeshFusion::Dataset_Fribourg datas;
    string shader_path = argv[1];

    bool use_folder_data=false;
    string image_folder;
    if(argc==3) {
        image_folder = argv[2];
        use_folder_data=true;
        datas.create(image_folder);
    }


    //datas.display();


    std::cout << "Kinect Pixel Fusion Starting "<< std::endl;

    PixMeshFusion::InterfaceOpenNI cam(PixMeshFusion::FPS_30, PixMeshFusion::WIDTH_640,true);
    cam.start();
    if(!cam.isOK() && (argc<3)) {
        std::cout << "camera ko" << std::endl;
        return 0;
    }

    int w_depth = cam.getDepthWidth();
    int h_depth = cam.getDepthHeight();

    int scales=6;//0,1,...,5;

    int w = w_depth;
    int h = h_depth;
    //prepare mesh
    //necessary init glew to bind buffer
    int win = PixMeshFusion::GLSLShader::GLinit(argc,argv,"GLUT INIT Test",w,h);
    bool test=false;

    if(!cam.isOK()) {
        w=640;
        h=480;
    }

    PixMeshFusion::Engine engine(w,h,scales,shader_path,test);
    if(!engine.isOK())  return 1;

    int iter = 1000000000;
    int iter1=1;//1 working otherwise not working properly
    int iter2=1;//1 working otherwise not working properly


    for(int frame=0; frame <iter; ++frame)
    {
       // if(frame>0) {
            bool received_cam=false;
            PixMeshFusion::CamData camdata;
            if(!use_folder_data)
                camdata = cam.getColorDepthData(received_cam);
            else
                if(datas.not_ended())
                    camdata = datas.getColorDepthData();
                else
                    return 0;

            float start00 = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
            for (int i=0;i<iter1;i++) {
                engine.loadCameraData(camdata);

            }
            float end00 = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
            std::cout << "one iter transfer rate in micro seconds " << (end00 - start00)*1000.0/float(iter1) << std::endl;
            float start0 = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
            for(int i=0;i<iter2;i++) {
                engine.setGaussNewton(false);
                //std::cout << "NEW ICP " << std::endl;
                //engine.setClassical(false);
                //engine.minimizeEnergy();
                std::cout << "CLASSICAL ICP " << std::endl;
                engine.setGaussNewton(false);
                engine.setClassical(true);
                engine.minimizeEnergy();
                //std::cout << "GAUSS NEWTON " << std::endl;
                //engine.setGaussNewton(true);
                //engine.setClassical(true);
                //engine.minimizeEnergy();
            }
            float end0 = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
            std::cout << "one iter GRADIENT time in micro seconds " << (end0 - start0)*1000.0/float(iter2) << std::endl;
        //}
        cv::waitKey(1000);
    }
    glutDestroyWindow(win);
    return 1;
}


