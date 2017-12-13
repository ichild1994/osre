/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2017 OSRE ( Open Source Render Engine ) by Kim Kulling

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-----------------------------------------------------------------------------------------------*/
#include "AbstractRenderTest.h"
#include "RenderTestUtils.h"

#include <osre/Common/Logger.h>
#include <osre/RenderBackend/RenderBackendService.h>
#include <osre/RenderBackend/RenderCommon.h>
#include <osre/RenderBackend/ParticleGenerator.h>
#include <src/Engine/RenderBackend/OGLRenderer/OGLShader.h>
#include <osre/Scene/GeometryBuilder.h>
#include <osre/Scene/MaterialBuilder.h>
#include <cppcore/Random/RandomGenerator.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#ifdef OSRE_WINDOWS
#   include "GL/wglew.h"
#endif

namespace OSRE {
namespace RenderTest {

using namespace ::OSRE::RenderBackend;

const String VsSrc =
    "#version 400 core\n"
    "\n"
    "layout(location = 0) in vec3 position;	     // object space vertex position\n"
    "layout(location = 1) in vec3 normal;	     // object space vertex normal\n"
    "layout(location = 2) in vec3 color0;        // per-vertex colour\n"
    "\n"
    "// output from the vertex shader\n"
    "smooth out vec4 vSmoothColor;		//smooth colour to fragment shader\n"
    "\n"
    "// uniform\n"
    "uniform mat4 M[25];	// model matrix per instance\n"
    "uniform mat4 VP;	// combined modelview projection matrix\n"
    "\n"
    "void main() {\n"
    "    //assign the per-vertex color to vSmoothColor varying\n"
    "    vSmoothColor = vec4(color0,1);\n"
    "\n"
    "    //get the clip space position by multiplying the combined MVP matrix with the object space\n"
    "    //vertex position\n"
    "    gl_Position = VP*M[ gl_InstanceID ]*vec4(position,1);\n"
    "}\n";

const String FsSrc =
    "#version 400 core\n"
    "\n"
    "layout(location=0) out vec4 vFragColor; //fragment shader output\n"
    "\n"
    "//input form the vertex shader\n"
    "smooth in vec4 vSmoothColor;		//interpolated colour to fragment shader\n"
    "\n"
    "void main() {\n"
    "    //set the interpolated color as the shader output\n"
    "    vFragColor = vSmoothColor;\n"
    "}\n";

//-------------------------------------------------------------------------------------------------
///	@ingroup	Test
///
///	@brief
//-------------------------------------------------------------------------------------------------
class SwitchCmdBufferRenderTest : public AbstractRenderTest {
    TransformMatrixBlock m_transformMatrix;
    static const ui32 NumPts = 1000;
    //glm::vec3 m_col[ NumPts ];
    //glm::vec3 m_pos[ NumPts ];
    Geometry *m_ptGeo;
    ParticleGenerator *m_particeGen;

public:
    SwitchCmdBufferRenderTest()
    : AbstractRenderTest( "rendertest/SwitchCmdBufferRenderTest" )
    , m_ptGeo( nullptr )
    , m_particeGen( nullptr ) {
        // empty
    }

    virtual ~SwitchCmdBufferRenderTest() {
        delete m_particeGen;
        m_particeGen = nullptr;
    }

    bool onCreate( RenderBackendService *rbSrv ) override {
        rbSrv->sendEvent( &OnAttachViewEvent, nullptr );

        m_particeGen = new ParticleGenerator( rbSrv );
        m_particeGen->init( NumPts );

        m_transformMatrix.update();
        rbSrv->setMatrix( "MVP", m_transformMatrix.m_mvp );

        return true;
    }

    bool onRender( d32 timediff, RenderBackend::RenderBackendService *rbSrv ) override {

        m_particeGen->update( NumPts );

        return true;
    }
};

ATTACH_RENDERTEST( SwitchCmdBufferRenderTest )

} // Namespace RenderTest
} // Namespace OSRE