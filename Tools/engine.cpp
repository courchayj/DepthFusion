/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/

#include "engine.h"
using namespace PixMeshFusion;

/*********************************************************************************************************/
Engine::Engine(int w, int h,int scales,std::string shader_path, bool test) {
    test_ = test;
    w_=w;
    h_=h;
    created =false;

    z_only=false;
    y_only=false;
    x_only=false;
    tx_only=false;
    ty_only=false;
    tz_only=false;
    meshes.create(w,h,scales);
    reduction_driver.create(w,h,scales,shader_path,&meshes);
    if(!reduction_driver.isOK())
        return;
    //prepare shader
    bool compile = (pre_compute.completePreCompute(shader_path) && compute_gradient.completeCompute(shader_path) &&
                    compute_gradient_classical.completeComputeClassical(shader_path) && compute_visibility.completeVGF(shader_path) &&
                    compute_gradient_classical_depth.completeComputeClassicalDepth(shader_path) );

    if(compile) {
        std::cout << "Shaders compiled and linked" << std::endl;
        compute_gradient.printWorkGroupsCapabilities();
    }
    else {
        std::cout << "Shaders creation or link or compile failed" << std::endl;
        pre_compute.printLog();
        compute_gradient.printLog();
        compute_gradient_classical.printLog();
        compute_gradient_classical_depth.printLog();
        compute_visibility.printLog();
        return ;
    }

    if(test_) {
        compile = (vertices_compute.completeVerticesCompute(shader_path) && compute_depth_visibility.completeVGFDepth(shader_path));

        if(!compile) {
            vertices_compute.printLog();
        }
    }

    created=true;
    //loadCalibration(calibrationFile); FROM FILE BETTER WITH CALIBRATION
    //test SO3
    //PixMeshFusion::SO3 test;
    //float uw[6];
    //uw[0]=1.0f; uw[1]=0.0f; uw[2]=2.5f; uw[3]=1.5f; uw[4]=1.1f; uw[5]=6.15f;
    //float y[3];
    //y[0]=0.6;y[1]=0.1;y[0]=3.6;
    //test.updateSO3(uw);
    //test.computeRT();
    //test.computeJacobian(y);
    meshes.attachReprojectionProgram(&compute_visibility);
    meshes.attachComputeShaderProgram(&compute_gradient);
    meshes.attachComputeShaderClassicalProgram(&compute_gradient_classical);
    meshes.attachComputeShaderClassicalDepthProgram(&compute_gradient_classical_depth);
    //intrinsic Camera multi scale

    //meshes.loadCamera(570.3422241210938*640.0/float(w), 570.3422241210938*480.0/float(h), 0.0f,  319.5*640.0/float(w),239.5*480.0/float(h),w,h);//570.3422241210938
    //meshes.loadCamera(578.6596070000001*640.0/float(w), 577.688354*480.0/float(h), 0.0f,  315.515736*640.0/float(w),220.208543*480.0/float(h),w,h);//570.3422241210938
    meshes.loadCamera(567.6*640.0/float(w), 570.2*480.0/float(h), 0.0f,  324.7*640.0/float(w),250.1*480.0/float(h),w,h);//570.3422241210938
    meshes.unbindFrameBuffer();

    setClassical(false);
    setGaussNewton(false);
    //classical=false;
    count=0;

}
/*********************************************************************************************************/
void Engine::loadCameraData(CamData camdata) {
    meshes.setCameraIndex(1);//camera index 0 // can deal with several cameras //NOT USEFUL
    meshes.setActiveScale(0);//Important!!

    meshes.updateTime();//increment time
    meshes.retrieveTextureId();//from time get the right texture: map to time t or to time t plus one

    cam_data = camdata;
    meshes.loadTextureColorDepth(camdata);

    meshes.generateMipMap();//Multiscale gpu computation
}
/*********************************************************************************************************/
void Engine::SyntheticTransformtTotp1() {
    //attach depth time t to renderbuffer
    //meshes.bindFrameBuffer();//DONE IN ATTACH
    meshes.AttachDepthTimetToRenderBuffer();


    vertices_compute.use();
    meshes.setUniformInvertIntrinsicsCompute();
    meshes.setUniformIntrinsicsFullCompute();
    meshes.bindTextureDepthTimetp1(0,true);//0: binding in compute shader // for reading
    glDispatchCompute(meshes.getScaleWidth()/threadx,meshes.getScaleHeight()/thready,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    compute_depth_visibility.use();//before set param and variables!! not after!!
    //add intrinsic
    //add rotation
    meshes.setUniformIntrinsics();
    //meshes.setUniformRotation(3.3,1.0,0.0,0.0);//red rotation (BGR)
    //meshes.setUniformRotation(3.3,0.0,1.0,0.0);//green rotation
    //meshes.setUniformRotation(3.0,0.0,1.0,0.0);//blue rotation
    meshes.setUniformTranslation(0.001,0,0.0);//red translation
    //meshes.setUniformTranslation(0,0.001,0.0);//green translation
    //meshes.setUniformTranslation(0,0,0.001);//blue translation

    //meshes.setUniformTranslation(0,0,0);
    meshes.bindMesh();//texture already done

    bool display = false;
    meshes.drawMesh(display);
    meshes.unbindFrameBuffer();
}
/*********************************************************************************************************/
void Engine::minimizeEnergy() {
    if(meshes.started() && created) { //DO COMPUTATION

        //TO BE REMOVED
        z_only=false;
        y_only=false;
        x_only=false;
        tx_only=false;
        ty_only=false;
        tz_only=false;
        //Only at first iteration at each scale
        so3_lie.reinit();//init transfo to identity
        ///////////////////////////////////////////////////
        // Add Cameras
        active_scale=0;
        threadx = 16;
        thready = 16;
        //pre_compute do with different threadx thready sizes
        pre_computes();
        //compute_gradient do with different threadx thready sizes
        for(int i=0;i<MAX_ITER;i++) {
            //transfert so3 RT
            compute_gradients();
            //get gradients sum with pbo
            //glMemoryBarrier(GL_PIXEL_BUFFER_BARRIER_BIT);
            meshes.getW123(so3_lie.w123Energy);
            meshes.getW456(so3_lie.w456Dd);

            float mult=500000.0;
            //if((i==0) || (i==MAX_ITER-1))
                std::cout << "energy iter " << i << " " << mult * so3_lie.w123Energy[3] << std::endl;

            if(!gauss_newton)
                minimizeGradient(i,0);
            else
                minimizeGauss();

            so3_lie.computeRT();
            /*if(test_)*/
            //if((i==0) || (i==MAX_ITER-1))
                checkData();
                 count++;
        }


        for(int i=0;i<MAX_ITER_WHOLE;i++) {
            //transfert so3 RT
            compute_gradients_whole(i);
            //get gradients sum with pbo
            //glMemoryBarrier(GL_PIXEL_BUFFER_BARRIER_BIT);
            meshes.getW123(so3_lie.w123Energy);
            meshes.getW456(so3_lie.w456Dd);

            float mult=500000.0;
            //if((i==0) || (i==MAX_ITER-1))
                std::cout << "energy DEPTH iter " << i << " " << mult * so3_lie.w123Energy[3] << std::endl;

            if(!gauss_newton)
                minimizeGradient(i+1,1);
            else
                minimizeGauss();

            //so3_lie.computeRT();
            /*if(test_)*/
            //if((i==0) || (i==MAX_ITER-1))
                checkData();
                 count++;
        }
    }

}
/*********************************************************************************************************/
void Engine::minimizeGradient(int iter,int step) {
    if(iter==0) {
        initStep(so3_lie.w123Energy,so3_lie.w456Dd,step);
    }

    improveStep(so3_lie.w123Energy,so3_lie.w456Dd);
    clampOmega(so3_lie.w123Energy,so3_lie.w456Dd);
    clampEnergy(so3_lie.w123Energy,so3_lie.w456Dd);

    //multiply by scaling speed factors
    rescaleOmega(so3_lie.w123Energy,so3_lie.w456Dd);
}
/*********************************************************************************************************/
void Engine::minimizeGauss() {
    meshes.setActiveScaleEnergy(0);
    float* dt1 = meshes.getTextureW123Energy();
    cv::Mat fr1;
    fr1.create(meshes.getScaleEnergyHeight(),meshes.getScaleEnergyWidth(), CV_32FC4);
    memcpy( fr1.data, (uchar*)dt1, 4*meshes.getScaleEnergyHeight()*meshes.getScaleEnergyWidth()*sizeof(float) );

    float* dt = meshes.getTextureW456Energy();
    cv::Mat fr;
    fr.create(meshes.getScaleEnergyHeight(),meshes.getScaleEnergyWidth(), CV_32FC4);
    memcpy( fr.data, (uchar*)dt, 4*meshes.getScaleEnergyHeight()*meshes.getScaleEnergyWidth()*sizeof(float) );


    cv::Mat planes1[4];
    cv::Mat planes[4];
    cv::Mat r[6];
    split(fr1,planes1);
    split(fr,planes);
    cv::Mat e =  planes1[3].reshape(1,fr1.rows*fr1.cols);
    r[0] = planes1[0].reshape(1,fr1.rows*fr1.cols);
    r[1] = planes1[1].reshape(1,fr1.rows*fr1.cols);
    r[2] = planes1[2].reshape(1,fr1.rows*fr1.cols);
    r[3] = planes[0].reshape(1,fr1.rows*fr1.cols);
    r[4] = planes[1].reshape(1,fr1.rows*fr1.cols);
    r[5] = planes[2].reshape(1,fr1.rows*fr1.cols);

    cv::Mat J;
    cv::Mat Jt;
    cv::hconcat(r,6,J);
    cv::Mat pinv;
    //std::cout << "rows1" << r[0].rows << std::endl;
    //std::cout << "cols1" << r[0].cols << std::endl;

    //std::cout << "rows" << J.rows << std::endl;
    //std::cout << "cols" << J.cols << std::endl;
    cv::transpose(J,Jt);
    cv::invert((Jt * J),pinv,cv::DECOMP_SVD);//pseudo invert
    cv::Mat omega = - pinv * Jt * e;

    float* w123 = so3_lie.w123Energy;
    float* w456 = so3_lie.w456Dd;
    w123[0]= omega.at<float>(0,0);
    w123[1]= omega.at<float>(0,1);
    w123[2]= omega.at<float>(0,2);
    w456[0]= omega.at<float>(0,3);
    w456[1]= omega.at<float>(0,4);
    w456[2]= omega.at<float>(0,5);


    /*
    alpha123[0]=1.0;
    alpha123[1]=1.0;
    alpha123[2]=1.0;
    alpha456[0]=1.0;
    alpha456[1]=1.0;
    alpha456[2]=1.0;
    clampOmega(w123,w456);
    rescaleOmega(w123,w456);
    */
    //free memory
    fr1.release();
    delete dt1;
    fr.release();
    delete dt;
}
/*********************************************************************************************************/
void Engine::improveStep(float w123[4],float w456[4]) {

    for(int i=0;i<3;i++) {
        if(old_w123[i]*w123[i] < 0)
           alpha123[i] /= DIVIDE_MOTION;
        else if( -alpha123[i]<-MOTION_MAX_INCREASE*alpha123initial[i])
            alpha123[i] *= MULTIP_MOTION;

        if(old_w456[i]*w456[i] < 0)
            alpha456[i] /= DIVIDE_MOTION;
        else if( -alpha456[i]<-MOTION_MAX_INCREASE*alpha456initial[i])
            alpha456[i] *= MULTIP_MOTION;


        old_w123[i]=w123[i];
        old_w456[i]=w456[i];


        if(old_energy <=(w123[3]*ENERGY_MAX_INCREASE)) {
            alpha123[i] /= DIVIDE_ENERGY;
            alpha456[i] /= DIVIDE_ENERGY;
        }
        else if((old_energy > w123[3])){
            if(-alpha456[i]<-MOTION_MAX_INCREASE*alpha456initial[i]) alpha456[i] *= MULTIP_ENERGY;
            if(-alpha123[i]<-MOTION_MAX_INCREASE*alpha123initial[i]) alpha123[i] *= MULTIP_ENERGY;
        }
    }
    //std::cout << "alpha " << alpha456[0] << std::endl;

    old_energy = w123[3];

}
/*********************************************************************************************************/
void Engine::rescaleOmega(float w123[4],float w456[4]) {
    w123[0]*=alpha123[0];
    w123[1]*=alpha123[1];
    w123[2]*=alpha123[2];

    w456[0]*=alpha456[0];
    w456[1]*=alpha456[1];
    w456[2]*=alpha456[2];

    //TO BE REMOVED
    if(z_only) {
        w123[0]=0.0;
        w123[1]=0.0;
        w123[2]=0.0;

        w456[0]=0.0;
        w456[1]=0.0;
    }
    if(y_only) {
        w123[0]=0.0;
        w123[1]=0.0;
        w123[2]=0.0;

        w456[0]=0.0;
        w456[2]=0.0;
    }
    if(x_only) {
        w123[0]=0.0;
        w123[1]=0.0;
        w123[2]=0.0;

        w456[1]=0.0;
        w456[2]=0.0;
    }
    if(tx_only) {
        //w123[0]=0.0;
        w123[1]=0.0;
        w123[2]=0.0;

        w456[0]=0.0;
        w456[1]=0.0;
        w456[2]=0.0;
    }
    if(ty_only) {
        w123[0]=0.0;
        //w123[1]=0.0;
        w123[2]=0.0;

        w456[0]=0.0;
        w456[1]=0.0;
        w456[2]=0.0;
    }
    if(tz_only) {
        w123[0]=0.0;
        w123[1]=0.0;
        //w123[2]=0.0;

        w456[0]=0.0;
        w456[1]=0.0;
        w456[2]=0.0;
    }
}
/*********************************************************************************************************/
bool Engine::clampEnergy(float w123[4],float w456[4]) {
    float energy = w123[3];
    float w0 = alpha123[0]*w123[0];
    float w1 = alpha123[1]*w123[1];
    float w2 = alpha123[2]*w123[2];
    float w3 = alpha456[0]*w456[0];
    float w4 = alpha456[1]*w456[1];
    float w5 = alpha456[2]*w456[2];

    bool clamped=false;

    float sum1 = -w0*w123[0] - w1*w123[1] - w2*w123[2] ;//because alpha<0
    float sum2 = -w3*w456[0] - w4*w456[1] - w5*w456[2];

    float delta1 =  energy / (max(sum1,MINI_GRADIENT)*STEP_CLAMP_ENERGY);
    float delta2 =  energy / (max(sum2,MINI_GRADIENT)*STEP_CLAMP_ENERGY);

    if(delta1 <1.0) {
        alpha123[0]*= delta1;
        alpha123[1]*= delta1;
        alpha123[2]*= delta1;
        clamped=true;
    }
    if(delta2 <1.0) {
        alpha456[0]*= delta2;
        alpha456[1]*= delta2;
        alpha456[2]*= delta2;
        clamped=true;
    }
    return clamped;
}
/*********************************************************************************************************/
bool Engine::clampOmega(float w123[4],float w456[4]) {
    float f = meshes.getFocal();
    float dp1 = abs(f*alpha123[0]*w123[0]/FAR_CLAMP)/CLAMP;//d(X/Z)/dX
    float dp2 = abs(f*alpha123[1]*w123[1]/FAR_CLAMP)/CLAMP;//d(Y/Z)/dY
    float dp3 = abs(f*alpha123[2]*w123[2]/FAR_CLAMP)/CLAMP;//d(X/Z)/dZ =dZ X / Z2
    float dp4 = abs(f*alpha456[0]*w456[0]/(FAR_CLAMP*FAR_CLAMP))/CLAMP;//X / Z2 dZ = XY/Z2 //FAR_CLAMP*FAR_CLAMP
    float dp5 = abs(f*alpha456[1]*w456[1]/(FAR_CLAMP*FAR_CLAMP))/CLAMP;   //FAR_CLAMP*FAR_CLAMP
    float dp6 = abs(f*alpha456[2]*w456[2]/(FAR_CLAMP*FAR_CLAMP))/CLAMP;//X/Z dX /Z //FAR_CLAMP*FAR_CLAMP

    //rotation * FAR_CLAMP
    bool clamped=false;

    if( (dp1>1.0) || (dp2>1.0) || (dp3>1.0) || (dp4>1.0) ||(dp5>1.0) ||(dp6>1.0) )
        clamped=true;

    if(dp1>1.0)    alpha123[0] /= dp1;
    if(dp2>1.0)    alpha123[1] /= dp2;
    if(dp3>1.0)    alpha123[2] /= dp3;

    if(dp4>1.0)    alpha456[0] /= dp4;
    if(dp5>1.0)    alpha456[1] /= dp5;
    if(dp6>1.0)    alpha456[2] /= dp6;

    return clamped;
}
/*********************************************************************************************************/
void Engine::initStep(float w123[4],float w456[4],int step) {
    float energy = w123[3];
    float w0 = w123[0];
    float w1 = w123[1];
    float w2 = w123[2];
    float w3 = w456[0];
    float w4 = w456[1];
    float w5 = w456[2];

    //TO BE REMOVED
    if(z_only) {
        w0 = 0.0;
        w1 = 0.0;
        w2 = 0.0;
        w3 = 0.0;
        w4 = 0.0;
    }
    if(y_only) {
        w0 = 0.0;
        w1 = 0.0;
        w2 = 0.0;
        w3 = 0.0;
        w5 = 0.0;
    }
    if(x_only) {
        w0 = 0.0;
        w1 = 0.0;
        w2 = 0.0;
        w4 = 0.0;
        w5 = 0.0;
    }
    if(tx_only) {
        //w0=0.0;
        w1=0.0;
        w2=0.0;

        w3=0.0;
        w4=0.0;
        w5=0.0;
    }
    if(ty_only) {
        w0=0.0;
        //w1=0.0;
        w2=0.0;

        w3=0.0;
        w4=0.0;
        w5=0.0;
    }
    if(tz_only) {
        w0=0.0;
        w1=0.0;
        //w2=0.0;

        w3=0.0;
        w4=0.0;
        w5=0.0;
    }

    float sum1 = w0*w0 + w1*w1 + w2*w2 ;
    float sum2 = w3*w3 + w4*w4 + w5*w5;
    float alpha1=1.0f;
    float alpha2=1.0f;

    float reduce;
    if(step==0) reduce = STEP_REDUCE_ENERGY;
    else reduce = STEP_REDUCE_ENERGY_LAST;

    if(sum1 > MINI_GRADIENT) {
        alpha1 =  energy / (float(reduce)* sum1);
    }
    if(sum2 > MINI_GRADIENT) {
        alpha2 =  energy / (float(reduce)* sum2);
    }


    alpha123[0]=-alpha1 * max(w0*w0,MINI_GRADIENT) / max(sum1,MINI_GRADIENT);
    alpha123[1]=-alpha1 * max(w1*w1,MINI_GRADIENT) / max(sum1,MINI_GRADIENT);
    alpha123[2]=-alpha1 * max(w2*w2,MINI_GRADIENT) / max(sum1,MINI_GRADIENT);
    alpha456[0]=-alpha2 * max(w3*w3,MINI_GRADIENT) / max(sum2,MINI_GRADIENT);
    alpha456[1]=-alpha2 * max(w4*w4,MINI_GRADIENT) / max(sum2,MINI_GRADIENT);
    alpha456[2]=-alpha2 * max(w5*w5,MINI_GRADIENT) / max(sum2,MINI_GRADIENT);

    alpha123initial[0]=alpha123[0];
    alpha123initial[1]=alpha123[1];
    alpha123initial[2]=alpha123[2];

    alpha456initial[0]=alpha456[0];
    alpha456initial[1]=alpha456[1];
    alpha456initial[2]=alpha456[2];

    for(int i=0;i<3;i++) {
        old_w123[i]=0.0;
        old_w456[i]=0.0;
    }

    old_energy = w123[3];
}
/*********************************************************************************************************/
void Engine::pre_computes() {
    meshes.setActiveScale(active_scale);//Important!!

    compute_visibility.use();//important before uniform transfer!!
    meshes.setUniformIntrinsics();
    //meshes.setUniformRotation(angle,0.1,-0.12,1.0);
    //meshes.setUniformRotation(0.0,0.0,1.0,0.0);
    if(!classical)
        compute_gradient.use();//important before uniform transfer!!
    else
        compute_gradient_classical.use();

    meshes.setUniformInvertIntrinsicsCompute();
    meshes.setUniformIntrinsicsFullCompute();
    pre_compute.use();
    meshes.setUniformInvertIntrinsicsCompute();//do pre_compute stuff

    compute_gradient_classical_depth.use();
    meshes.setUniformInvertIntrinsicsComputeDepth();
    meshes.setUniformIntrinsicsFullComputeDepth();
    //TEST
    /////////////////////
    if(test_)   SyntheticTransformtTotp1();
    //////////////////////

    /////////////////////////////////////////////////////////////////
    //Pre compute normals
    pre_compute.use();
    meshes.bindTextureDepthTimet(0,true);//0: binding in pre_compute shader // for reading
    meshes.bindTextureNormalDdepth(1,false);
    meshes.bindTextureNormalzVisible(2,false);
    meshes.bindTextureDeltaDepthTimetp1(3,false);//binding 2 // for writing

    glDispatchCompute(meshes.getScaleWidth()/threadx,meshes.getScaleHeight()/thready,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    ///////////////////////////////////////////////////
}
/*********************************************************************************************************/
void Engine::compute_gradients_whole(int iter){
    compute_gradient_classical_depth.use();
    meshes.setActiveScale(active_scale);//Important!!
    meshes.bindTextureDeltaDepthTimetp1(2,false);//binding 2 // for writing
    meshes.bindTextureAlpha(6,false);//binding 2 // for writing
    meshes.bindTextureDepthTimetp1(0,true);//0: binding in compute shader // for reading
    meshes.bindTextureDepthTimet(1,true);//1: binding in compute shader // for reading
    meshes.bindTextureNormalDdepth(4,true);
    meshes.bindTextureNormalzVisible(5,true);
    meshes.bindMeshForComputeShader(3);//3: number binding  written inside compute shader
    meshes.bindEnergy123ForComputeShader(4);
    meshes.bindEnergy456ForComputeShader(5);

    meshes.setUniformRotationComputeDepth(so3_lie.getRT());//add rotation
    meshes.setUniformIntComputeDepth(iter);
    glDispatchCompute(meshes.getScaleWidth()/threadx,meshes.getScaleHeight()/thready,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    compute_visibility.use();//before set param and variables!! not after!!
    //so3_lie.printRT();
    meshes.setUniformRotation(so3_lie.getRT());
    //meshes.attachReprojectionProgramColorToIntAndBind(0);//DEPRECATED
    //meshes.attachReprojectionProgramDepthToIntAndBind(1);//DEPRECATED
    //meshes.attachReprojectionProgramEnergyToIntAndBind(2);//DEPRECATED

    meshes.unbindFrameBuffer();

    meshes.AttachEnergyToRenderBuffer();
    meshes.bindMesh();//texture already done
    meshes.bindEnergy();
    bool display = false;
    meshes.drawMesh(display);

    meshes.unbindFrameBuffer();
    // REDUCE SUM WITH DRIVER
    reduction_driver.set_active_scale(active_scale);
    reduction_driver.reduce();
    cv::waitKey(100);
}
/*********************************************************************************************************/
void Engine::compute_gradients(){
    if(!classical)
        compute_gradient.use();//important before uniform transfer!!
    else
        compute_gradient_classical.use();
    meshes.setActiveScale(active_scale);//Important!!
    meshes.bindTextureDepthTimetp1(0,true);//0: binding in compute shader // for reading
    meshes.bindTextureDepthTimet(1,true);//1: binding in compute shader // for reading
    meshes.bindTextureNormalDdepth(4,true);
    meshes.bindTextureNormalzVisible(5,true);
    meshes.bindMeshForComputeShader(3);//3: number binding  written inside compute shader
    meshes.bindEnergy123ForComputeShader(4);
    meshes.bindEnergy456ForComputeShader(5);

    meshes.setUniformRotationCompute(so3_lie.getRT());//add rotation

    glDispatchCompute(meshes.getScaleWidth()/threadx,meshes.getScaleHeight()/thready,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    compute_visibility.use();//before set param and variables!! not after!!
    //so3_lie.printRT();
    meshes.setUniformRotation(so3_lie.getRT());
    //meshes.attachReprojectionProgramColorToIntAndBind(0);//DEPRECATED
    //meshes.attachReprojectionProgramDepthToIntAndBind(1);//DEPRECATED
    //meshes.attachReprojectionProgramEnergyToIntAndBind(2);//DEPRECATED

    meshes.unbindFrameBuffer();

    meshes.AttachEnergyToRenderBuffer();
    meshes.bindMesh();//texture already done
    meshes.bindEnergy();
    bool display = false;
    meshes.drawMesh(display);

    meshes.unbindFrameBuffer();
    // REDUCE SUM WITH DRIVER
    reduction_driver.set_active_scale(active_scale);
    reduction_driver.reduce();
}
/*********************************************************************************************************/
void Engine::checkData() {

    cv::Mat frame1;
    frame1.create(h_, w_, CV_16UC1);
    memcpy( frame1.data, (uchar*)cam_data.depthData, w_*h_*sizeof(openni::DepthPixel) );

    //glMemoryBarrier(GL_PIXEL_BUFFER_BARRIER_BIT);
    float* Datat;
    //CPU GPU Synchronization slow
    meshes.setActiveScaleEnergy(0);//IMPORTANT
    Datat = meshes.getTextureNormal();
    //Datat = meshes.getTextureDepth();//CPU GPU Synchronization slow
    cv::Mat framei;
    framei.create(h_, w_, CV_32FC4);
    memcpy( framei.data, (uchar*)Datat, w_*h_*sizeof(float)*4 );

    unsigned short* Datat2;
    Datat2 = meshes.getTextureDepthTimetp1();
    cv::Mat framei2;
    framei2.create(h_, w_, CV_16UC1);
    memcpy( framei2.data, (uchar*)Datat2, w_*h_*sizeof(unsigned short) );

    meshes.setActiveScaleEnergy(0);
    float* dt = meshes.getTextureW123Energy();
    cv::Mat fr;
    fr.create(meshes.getScaleEnergyHeight(),meshes.getScaleEnergyWidth(), CV_32FC4);
    memcpy( fr.data, (uchar*)dt, 4*meshes.getScaleEnergyHeight()*meshes.getScaleEnergyWidth()*sizeof(float) );

    cv::Mat planes[4];
    split(fr,planes);

    float mult=1000.0;//500000000.0;//3.0;//50000.0//1000.0
    //cv::imshow("normal", 5*framei);
    cv::imshow("energy", mult*planes[3]);

    ostringstream counter;
    string result;
    counter << std::setfill('0') << std::setw(5) << count ;
    result=counter.str();
    if(count <10000)
        cv::imwrite("/home/courchay/Documents/dataset/img" + result + ".png",255*mult*planes[3]);
    cout << "saved" << result << endl;
    //cv::imshow("deptht", 256*framei);
    //cv::imshow("depthtp", 256*framei2);

    //cv::waitKey(0);

    float sumx=0.0;
    float sumy=0.0;
    float sumz=0.0;
    for(int i=0;i<fr.rows;i++) {
        for(int j=0;j<fr.cols;j++) {
            sumx+=fr.at<cv::Vec4f>(i,j)[0];
            sumy+=fr.at<cv::Vec4f>(i,j)[1];
            sumz+=fr.at<cv::Vec4f>(i,j)[2];
        }
    }

    //free memory
    delete Datat;
    delete Datat2;
    delete dt;
    frame1.release();
    framei.release();
    framei2.release();
    fr.release();

    cv::waitKey(1);
}
/*********************************************************************************************************/


