#version 400 core
#extension GL_ARB_shading_language_include : require

// RenderVertex layout
layout(location = 0) in vec3 position;	  // object space vertex position
layout(location = 1) in vec3 normal;	  // object space vertex normal
layout(location = 2) in vec3 color0;     // per-vertex diffuse colour
layout(location = 3) in vec2 texcoord0;  // per-vertex tex coord, stage 0

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
