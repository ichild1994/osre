#include Common.glsl

out vec2 TexCoords;

uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(position.x, position.y, 0, 1.0);
    TexCoords = texcoord0;\n"
}
