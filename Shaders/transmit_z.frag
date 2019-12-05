#version 430
in float zFrag;

layout( location = 0 ) out float FragColor;

void main() {
    FragColor =  zFrag;
}

