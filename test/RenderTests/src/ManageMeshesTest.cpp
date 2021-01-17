#include "AbstractRenderTest.h"
#include "RenderTestUtils.h"

#include <osre/RenderBackend/Mesh.h>
#include <osre/RenderBackend/RenderBackendService.h>
#include <osre/Scene/MeshBuilder.h>

namespace OSRE {
namespace RenderTest {

using namespace ::OSRE::RenderBackend;

class ManageMeshesTest : public AbstractRenderTest {
public:
    ManageMeshesTest() :
            AbstractRenderTest("rendertest/ManageMeshesTest") {
        // empty
    }

    ~ManageMeshesTest() override {
        // empty
    }

    bool onCreate(RenderBackendService *rbSrv) override {
        rbSrv->sendEvent(&OnAttachViewEvent, nullptr);

        Scene::MeshBuilder myBuilder;
        Mesh *mesh = myBuilder.allocTriangles(VertexType::ColorVertex, BufferAccessType::ReadOnly).getMesh();

        rbSrv->beginPass(PipelinePass::getPassNameById(RenderPassId));
        {
            rbSrv->beginRenderBatch("b1");
            {
                rbSrv->addMesh(mesh, 0);
            }
            rbSrv->endRenderBatch();
        }
        rbSrv->endPass();

        return true;
    }

    /*bool onRender(RenderBackendService *rbSrv) override {
        Scene::MeshBuilder myBuilder;
        myBuilder.allocTriangles(VertexType::ColorVertex, BufferAccessType::ReadOnly);
        Mesh *mesh = myBuilder.allocTriangles(VertexType::ColorVertex, BufferAccessType::ReadOnly)
                             .getMesh();
        rbSrv->beginPass(PipelinePass::getPassNameById(RenderPassId));
        {
            rbSrv->beginRenderBatch("b1");
            {
                rbSrv->addMesh(mesh, 0);
            }
        }
        return true;
    }*/
};

ATTACH_RENDERTEST(ManageMeshesTest)


} // namespace RenderTest
} // namespace RenderTest
