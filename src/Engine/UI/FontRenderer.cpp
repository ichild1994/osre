#include "FontRenderer.h"

#include <osre/Scene/MeshBuilder.h>
#include <osre/Common/Logger.h>
#include <osre/RenderBackend/Mesh.h>
#include <osre/RenderBackend/RenderBackendService.h>
#include <osre/RenderBackend/RenderCommon.h>
#include <osre/Scene/MaterialBuilder.h>
#include <osre/RenderBackend/Pipeline.h>

#include <osre/UI/Widget.h>
#include <osre/Math/BaseMath.h>

namespace OSRE {
namespace UI {

using namespace OSRE::RenderBackend;
using namespace OSRE::Scene;

namespace {

static const c8 *Tag = "FontRenderer";

}
const String VertexShaderFont = 
    "#version 330 core\n"
    "layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
    "out vec2 TexCoords;\n"
    "\n"
    "uniform mat4 projection;\n"
    "\n void main() {\n"
    "    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
    "    TexCoords = vertex.zw;\n"
    "}\n";

const String FragmentShaderFont =
        "#version 330 core\n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"
        "\n"
        "uniform sampler2D text;\n"
        "uniform vec3 textColor;\n"
        "\n"
        "void main() {\n"
        "    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
        "    color = vec4(textColor, 1.0) * sampled;\n"
        "}\n";

FontRenderer::FontTextEntry::FontTextEntry() :
        mText(),
        mMesh( nullptr ){
    // empty
}

static void insertTextEntry(ui32 id, Mesh *mesh, const String &text, FontRenderer::TextBoxHashMap &textBoxes) {
    FontRenderer::FontTextEntry *entry = new FontRenderer::FontTextEntry;
    entry->mMesh = mesh;
    entry->mText = text;
    textBoxes.insert(id, entry);
}

FontRenderer::FontRenderer() :
        m_transformMatrix(),
        mTextBoxes(),
        mTbArray() {
    // empty
}

FontRenderer::~FontRenderer() {
    clear();
}

void FontRenderer::AddRenderText(ui32 id, ui32 x, ui32 y, const String &text, RenderBackendService *rbSrv) {
    if (text.empty() || nullptr == rbSrv) {
        return;
    }

    f32 xTrans(0), yTrans(0);
    UI::WidgetCoordMapping::mapPosToWorld(x, y, xTrans, yTrans);
    MeshBuilder meshBuilder;
    if (!mTextBoxes.hasKey(id)) {
        Mesh *mesh = meshBuilder.allocTextBox(xTrans, yTrans, 1.0f, text, BufferAccessType::ReadWrite)
                             .getMesh();
        rbSrv->addMesh(mesh, 0);
        insertTextEntry(id, mesh, text, mTextBoxes);
        return;
    }
    FontTextEntry *entry = nullptr;
    if (!mTextBoxes.getValue(id, entry)) {
        return;
    }
        
    if (nullptr == entry) {
        return;
    }

    if (entry->mText != text) {
        if (text.size() > entry->mText.size()) {
            meshBuilder.allocTextBox(xTrans, yTrans, 1.0f, text, BufferAccessType::ReadWrite);
            entry->mMesh = meshBuilder.getMesh();
        } else {
            MeshBuilder::updateTextBox(entry->mMesh, 1.0f, text);
        }
        rbSrv->updateMesh(entry->mMesh);
    }
}

void FontRenderer::clear() {
    if (mTextBoxes.isEmpty()) {
        return;
    }

    for (ui32 i = 0; i < mTbArray.size(); i++) {
        if (nullptr != mTbArray[i]) {
            delete mTbArray[i];
        }
    }
    mTextBoxes.clear();
    mTextBoxes.init(100);
    mTbArray.resize(0);
    mTextBoxes.init(100);
}

} // namespace UI
} // namespace OSRE
