# OSRE - An Open Source Render Experience

[![Join the chat at https://gitter.im/kimkulling/osre](https://badges.gitter.im/kimkulling/osre.svg)](https://gitter.im/kimkulling/osre?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
Another open-source render experience. feel free to play around with it!

# Supported platforms:
- Windows: 
  + OpenGL-Renderbackend
  + Vulkan-Renderbackend
- Linux ( [![Build Status](https://travis-ci.org/kimkulling/osre.png)](https://travis-ci.org/kimkulling/osre) ): 
  + OpenGL-renderbackend

# How to start
## What you need to install
- A Linux or Windows-PC with a more or less modern GPU
- cmake to genreate the build environment for your platform ( see www.cmake.org )
- SDL2.0 ( see https://www.libsdl.org/download-2.0.php )

## How to build it
Clone the repo via git from our project space on github.
```
> cd projects
> git clone https://github.com/kimkulling/osre.git
```
Now update all used submodule, which are usep by OSRE:
```
> git submodule init
> git submodule update -r
```
Generate a build environment of your choise:
> cmake CMakeLists.txt

for linux:
> make

for Windows: Use the generated Visual studio project files.

## Featurelist
- OpenGL4.x-Renderer:
   + Multithreaded rendnering via separate render-thread
   + Primitive rendering
   + Texture mapping
   + Instanced rendering
   + Font rendering

- Vulkan-Renderer:
   + Experimental

## Get involved
If you want to contribute just use the github project page or reach us via Gitter:
[![Join the chat at https://gitter.im/kimkulling/osre](https://badges.gitter.im/kimkulling/osre.svg)](https://gitter.im/kimkulling/osre?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## What is cooking
- Working on Vulkan support
- If you want to see what is currently under development or what is planned next you can take a look into our planning board: 
[OSRE Planning Board](https://trello.com/b/AJEAv98P/osre)

## Reference doc
Check our doxygen reference doc: http://kimkulling.github.io/osre/doc/html/index.html

# Samples
## Sample 1: Hello world
This example will show you how to create a windo and add a simple stage to it.
### [01-helloworld](https://github.com/kimkulling/osre/blob/master/samples/00_HelloWorld/)

# Running the render-tests
After building the engine you can find the render-tests in the binary-folder. To run the tests just run:
- for Windows:
> .\osre-rendertest.exe

- for Linux:
> ./osre-rendertest

You can terminate the current test-fixure by pressing any key...
