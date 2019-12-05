#version 430
layout(location = 0) in vec4 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;

layout(location = 2) in vec4 energy123;
layout(location = 3) in vec4 energy456;

layout(location = 4)uniform mat4 intrinsics;
layout(location = 5)uniform mat4 rotation;

out vec2 TexCoord;
out vec4 Energy123;
out vec4 Energy456;

void main()
{
gl_Position =  /*intrinsics * rotation **/ VertexPosition; //computed in geometry shader

TexCoord = VertexTexCoord;
Energy123 = energy123;
Energy456 = energy456;

}
