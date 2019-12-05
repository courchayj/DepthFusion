#version 430
in vec2 TexCoordFrag;
in vec4 Energy123Frag;
in vec4 Energy456Frag;

//uniform sampler2D TexEnergy;
//uniform sampler2D TexColor;
uniform sampler2D TexDepth;

layout( location = 0 ) out vec4 FragColor123;
layout( location = 1 ) out vec4 FragColor456;

void main() {
    //FragColor123 =  10.0*texture(TexDepth, TexCoordFrag);//level resolution 0
    //FragColor456 =  10.0*texture(TexDepth, TexCoordFrag);//level resolution 0
    //FragColor =  texture(TexColor, TexCoordFrag);//level resolution 2


    //FragColor =  5000*textureLod(TexEnergy, TexCoordFrag,0);//level resolution 2 Translation
    //FragColor = 50000* textureLod(TexEnergy, TexCoordFrag,0);//level resolution 2 Rotation
    FragColor123 = Energy123Frag;//level resolution 2 Rotation
    FragColor456 = Energy456Frag;
}

