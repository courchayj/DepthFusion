#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 TexCoord[];
in vec4 Energy123[];
in vec4 Energy456[];
out vec2 TexCoordFrag;
out vec4 Energy123Frag;
out vec4 Energy456Frag;

layout(location = 4)uniform mat4 intrinsics;
layout(location = 5)uniform mat4 rotation;

const float discontinuity_thresh =0.0025;//0.003;//0.003
//0.003 KINECT
//0.00003 TUM NO RESCALE
void main(void)
{

    bool emit_line = ((gl_in[0].gl_Position.z > 0.0)&&(gl_in[1].gl_Position.z > 0.0)&&(gl_in[2].gl_Position.z > 0.0));
    bool emit_continuity = ((abs(gl_in[0].gl_Position.z - gl_in[1].gl_Position.z)<discontinuity_thresh)&&(abs(gl_in[0].gl_Position.z - gl_in[2].gl_Position.z)<discontinuity_thresh)&&(abs(gl_in[1].gl_Position.z - gl_in[2].gl_Position.z)<discontinuity_thresh));
    if(emit_line && emit_continuity) {
        gl_Position = intrinsics * rotation * gl_in[0].gl_Position;
        TexCoordFrag = TexCoord[0];
        Energy123Frag = Energy123[0];
        Energy456Frag = Energy456[0];
        EmitVertex();
        gl_Position = intrinsics * rotation * gl_in[1].gl_Position;
        TexCoordFrag = TexCoord[1];
        Energy123Frag = Energy123[1];
        Energy456Frag = Energy456[1];
        EmitVertex();
        gl_Position = intrinsics * rotation * gl_in[2].gl_Position;
        TexCoordFrag = TexCoord[2];
        Energy123Frag = Energy123[2];
        Energy456Frag = Energy456[2];
        EmitVertex();
    }
    EndPrimitive();
}
