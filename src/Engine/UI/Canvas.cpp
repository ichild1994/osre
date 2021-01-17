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
#include <osre/Platform/AbstractWindow.h>
#include <osre/RenderBackend/RenderBackendService.h>
#include <osre/RenderBackend/RenderCommon.h>
#include <osre/UI/Canvas.h>
#include <osre/UI/FocusControl.h>

#include "UIRenderUtils.h"

namespace OSRE {
namespace UI {

using namespace ::OSRE::RenderBackend;
using namespace ::OSRE::Platform;

Canvas::Canvas(const String &name, ui32 x, ui32 y, ui32 width, ui32 height) :
        Widget(name, nullptr),
        m_surface(nullptr),
        m_focusControl(nullptr) {
    Widget::setRect(x, y, width, height);
    m_focusControl = new FocusControl;
}

Canvas::~Canvas() {
    m_surface = nullptr;

    delete m_focusControl;
    m_focusControl = nullptr;
}

void Canvas::setSurface(AbstractWindow *surface) {
    if (surface == m_surface) {
        return;
    }

    m_surface = surface;
    if (nullptr == surface) {
        return;
    }

    WindowsProperties *winProperties(surface->getProperties());
    if (nullptr == winProperties) {
        return;
    }

    Rect2ui dim;
    winProperties->getDimension(dim);
    WidgetCoordMapping::init(dim);
}

const TransformMatrixBlock &Canvas::getTransform() const {
    return m_transformMatrix;
}

FocusControl *Canvas::getFocusControl() const {
    return m_focusControl;
}

void Canvas::onLayout() {
}

void Canvas::onRender(UiRenderCmdCache &renderCmdCache, RenderBackendService *rbSrv) {
    if (nullptr == m_surface) {
        return;
    }

    // push 2D render mode
    const Rect2ui &r = Widget::getRect();
    m_transformMatrix.m_projection = glm::ortho((i64)r.getX1(), (i64)r.getWidth(), (i64)r.getHeight(), (i64)r.getY1());
    m_transformMatrix.m_view = glm::mat4(1);
    m_transformMatrix.m_model = glm::rotate(m_transformMatrix.m_model, 0.01f, glm::vec3(1, 1, 0));
    m_transformMatrix.update();

    const size_t numChildren = getNumWidgets();
    if (0 == numChildren) {
        return;
    }

    for (size_t i = 0; i < numChildren; ++i) {
        Widget *currentChild(getWidgetAt(i));
        if (nullptr != currentChild) {
            currentChild->render(renderCmdCache, rbSrv);
        }
    }
}

void Canvas::onResize(ui32 x, ui32 y, ui32 w, ui32 h) {
    Rect2ui dim(x, y, w, h);
    WidgetCoordMapping::init(dim);
}

} // Namespace UI
} // Namespace OSRE
