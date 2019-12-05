#ifndef VARIABLES_H
#define VARIABLES_H

//update so3 to divide energy by 2 at first iter
//change direction divide by 2
//same direction and divided true => multiply by 1.5
#define DIVIDE_ENERGY 1.25//1.25
#define DIVIDE_MOTION 2.0//2.0
#define MULTIP_MOTION 1.5//1.5
#define MULTIP_ENERGY 1.125//1.125
#define MOTION_MAX_INCREASE 1000000000.0
#define ENERGY_MAX_INCREASE 1.1

#define MINI_GRADIENT 0.00000001f

#define FAR_CLAMP  3.0//3.0 CHANGE FOR TUM NO RESCALE
#define CLAMP 0.33
#define STEP_REDUCE_ENERGY 50.0 //50.0
#define STEP_REDUCE_ENERGY_LAST 400.0 //50.0

#define STEP_CLAMP_ENERGY 35.0 //25.0

#define MAX_ITER 100
#define MAX_ITER_WHOLE 25


#define OFFSET 0.5
#define NEAR 0.001 //0.001 KINECT //0.000! TUM NO RESCALE
#define FAR 1000.0//1000
#define DISCONTINUITY 0.0025
//0.003 KINECT
//0.00003 TUM NO RESCALE

#endif // VARIABLES_H
