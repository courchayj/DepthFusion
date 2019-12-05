#version 430
layout(location = 0) in vec4 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;

layout(location = 4)uniform mat4 intrinsics;
layout(location = 5)uniform mat4 rotation;

out float z;

void main()
{
gl_Position = /*intrinsics * rotation **/ VertexPosition; //computed in geometry shader

vec4 transformed = rotation * VertexPosition;
z = transformed.z;

}
