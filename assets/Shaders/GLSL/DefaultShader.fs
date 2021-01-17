layout(location=0) out vec4 vFragColor; //fragment shader output

//input form the vertex shader
smooth in vec4 vSmoothColor;		//interpolated colour to fragment shader
smooth in vec2 vUV;
uniform sampler2D tex0;

void main() {
    // set the interpolated color as the shader output
    vec4 texColor = texture( tex0, vUV );
    vFragColor = vSmoothColor * texColor;
    vFragColor = texture( tex0, vUV );
}

