#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in float z[];
out float zFrag;

layout(location = 4)uniform mat4 intrinsics;
layout(location = 5)uniform mat4 rotation;

const float discontinuity_thresh =0.0025;//0.003

void main(void)
{

    bool emit_line = ((gl_in[0].gl_Position.z > 0.0)&&(gl_in[1].gl_Position.z > 0.0)&&(gl_in[2].gl_Position.z > 0.0));
    bool emit_continuity = ((abs(gl_in[0].gl_Position.z - gl_in[1].gl_Position.z)<discontinuity_thresh)&&(abs(gl_in[0].gl_Position.z - gl_in[2].gl_Position.z)<discontinuity_thresh)&&(abs(gl_in[1].gl_Position.z - gl_in[2].gl_Position.z)<discontinuity_thresh));
    if(emit_line && emit_continuity) {
        gl_Position = intrinsics * rotation * gl_in[0].gl_Position;
        zFrag = z[0];
        EmitVertex();
        gl_Position = intrinsics * rotation * gl_in[1].gl_Position;
        zFrag = z[1];
        EmitVertex();
        gl_Position = intrinsics * rotation * gl_in[2].gl_Position;
        zFrag = z[2];
        EmitVertex();
    }
    EndPrimitive();
}
