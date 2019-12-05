/*
* This file was written as Class Tools for PixMeshFusion
* by Jerome Courchay (courchay@i3s.unice.fr)
*
*/

#ifndef STRUCTURES_H
#define STRUCTURES_H

namespace PixMeshFusion {
struct CamData {
    openni::DepthPixel* depthData;
    openni::RGB888Pixel* colorData;
};
}
#endif // CAMERA_H
