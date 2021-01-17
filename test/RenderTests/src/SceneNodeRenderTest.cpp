/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2020 OSRE ( Open Source Render Engine ) by Kim Kulling

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

#include <osre/RenderBackend/Mesh.h>
#include <osre/RenderBackend/RenderBackendService.h>
#include <osre/Scene/DbgRenderer.h>
#include <osre/Scene/MeshBuilder.h>

#include <iomanip>

namespace OSRE {
namespace RenderTest {

using namespace ::OSRE::RenderBackend;

//-------------------------------------------------------------------------------------------------
///	@ingroup	RenderTest
///
///	@brief  A scene-node-rendering test.
//-------------------------------------------------------------------------------------------------
class SceneNodeRenderTest final : public AbstractRenderTest {
    TransformMatrixBlock m_transformMatrix;

public:
    SceneNodeRenderTest() :
            AbstractRenderTest("rendertest/GeoModelMatrixRenderTest") {
        // empty
    }

    ~SceneNodeRenderTest() override {
        // empty
    }

    bool onCreate(RenderBackendService *rbSrv) override {
        rbSrv->sendEvent(&OnAttachViewEvent, nullptr);

        Scene::MeshBuilder myBuilder;
        Mesh *mesh1 = myBuilder.allocTriangles(VertexType::ColorVertex, BufferAccessType::ReadOnly)
            .getMesh();

        rbSrv->beginPass(PipelinePass::getPassNameById(RenderPassId));
        {
            rbSrv->beginRenderBatch("b1");
            {
                rbSrv->addMesh(mesh1, 0);

                myBuilder.allocTriangles(VertexType::ColorVertex, BufferAccessType::ReadOnly);
                Mesh *mesh2 = myBuilder.getMesh();
                rbSrv->addMesh(mesh2, 0);
            }
            rbSrv->endRenderBatch();
        }
        rbSrv->endPass();

        return true;
    }
};

ATTACH_RENDERTEST(SceneNodeRenderTest)

} // namespace RenderTest
} // namespace OSRE
