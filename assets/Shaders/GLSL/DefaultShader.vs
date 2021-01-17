#include Common.glsl

// output from the vertex shader
smooth out vec4 vSmoothColor;		//smooth colour to fragment shader
smooth out vec2 vUV;

void main() {
    //assign the per-vertex color to vSmoothColor varying
    vSmoothColor = vec4(color0,1);

    //get the clip space position by multiplying the combined MVP matrix with the object space
    //vertex position
    gl_Position = MVP*vec4(position,1);
    vSmoothColor = vec4( color0, 1 );
    vUV = texcoord0;
}
